#include "../include/core_service.hpp"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using std::condition_variable;
using std::mutex;
using std::thread;
using std::unique_lock;
using std::unique_ptr;

constexpr int NetworkCoreService::SOCKET_OPTION;

future<Response> LocalCoreService::execAction(unique_ptr<CoreAction> &&action) {
    future<Response> fut = action->getFuture();
    action->setCore(core);
    unique_lock<mutex> actionsLock(actionsMutex);
    actions.push(std::move(action));
    actionsLock.unlock();
    actionsCv.notify_one();
    return fut;
}

void LocalCoreService::execActionLoop() {
    while(!stopService.load()) {
        unique_lock<mutex> actionsLock(actionsMutex);
        if(actions.empty()) {
            actionsCv.wait(actionsLock);
        }

        if(!actions.empty()) {
            auto action{ std::move(actions.front()) };
            actions.pop();
            actionsLock.unlock();
            try {
                action->exec();
            } catch(const string& ex) {
                std::cerr << ex << std::endl;
            } catch(...) {
                std::cerr << "Unexpected exception in Exec Action Loop" << std::endl;
            }
        } else {
            actionsLock.unlock();
        }
    }
}

void LocalCoreService::start()  {
    execActionThread = thread{ &LocalCoreService::execActionLoop, this };
}

void LocalCoreService::stop() {
    stopService.store(true);
    actionsCv.notify_one();
    execActionThread.join();
}

void NetworkCoreService::start() {
    LocalCoreService::start();
    socketProcessingThread = thread{ &NetworkCoreService::processNextRequest, this };
    createServerSocket();
    acceptRequests();
}

void NetworkCoreService::stop() {
    LocalCoreService::stop();
    stopService.store(true);
    socketCv.notify_one();
    socketProcessingThread.join();
}

void NetworkCoreService::createServerSocket() {
    if((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        throw string{ "Socket creation failed" };
    }

    if(setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                  &SOCKET_OPTION, sizeof(SOCKET_OPTION))) {
        throw string{ "Setting socket options failed" };
    }

    socketAddr.sin_family = AF_INET;
    socketAddr.sin_addr.s_addr = INADDR_ANY;
    socketAddr.sin_port = htons(port);

    int socketAddrLen = sizeof(socketAddr);

    if(bind(serverFd, reinterpret_cast<struct sockaddr*>(&socketAddr), socketAddrLen) < 0) {
        throw string{ "Socket bind failed" };
    }

    if (listen(serverFd, connectionQueueSize) < 0) {
        throw string{ "Listen failed" };
    }
}

void NetworkCoreService::acceptRequests() {
    while(!stopService.load()) {
        int newSocket;
        int sockAddrLen = sizeof(struct sockaddr_in);
        if ((newSocket = accept(serverFd, reinterpret_cast<struct sockaddr *>(&socketAddr),
                     reinterpret_cast<socklen_t*>(&sockAddrLen))) < 0) {
            throw string{ "Accept failed" };
        }

        unique_lock<mutex> socketLock{ socketMutex };
        sockets.push(newSocket);
        socketLock.unlock();
        socketCv.notify_one();
    }
}

void NetworkCoreService::processNextRequest() {
    while (!stopService.load()) {
            unique_lock<mutex> socketLock{ socketMutex };
            if(sockets.empty()) {
            socketCv.wait(socketLock);
        }

        // Queue still may be empty even after wait() returned
        if(!sockets.empty()) {
            auto socket{ sockets.front() };
            sockets.pop();
            socketLock.unlock();
            try {
                processRequest(socket);
            } catch(const string& ex) {
                std::cerr << ex << std::endl;
            } catch(...) {
                std::cerr << "Unexpected exception in Process Next Request" << std::endl;
            }
        } else {
            socketLock.unlock();
        }
    }
}

void NetworkCoreService::processRequest(int socket) {
    auto data = readSocket(socket);
    close(socket);
    //std::cout << "processed: " << data.first << std::endl;
    //std::cout << data.second.get() << std::endl;
    for (size_t i = 0; i < data.first; ++i) {
        std::cout << *(data.second.get() + i);
    }
    std::cout << std::endl;
    // parse XML
    // init action
    // exec action
}

pair<size_t, unique_ptr<char>> NetworkCoreService::readSocket(int socket) {
    constexpr ssize_t dataLenBuffSize{ 9 };
    char dataLenBuff[dataLenBuffSize];

    auto readCount = read(socket, dataLenBuff, dataLenBuffSize);
    if(readCount != dataLenBuffSize) {
        throw string{ "Cannot read data length" };
    }

    auto dataLen = atoi(dataLenBuff);
    if(dataLen <= 0) {
        throw string{ "Data length should be positive" };
    }

    unique_ptr<char> dataBuff{ new char[dataLen] };
    char *dataBuffWrPos = dataBuff.get();
    char *dataBuffEndPos = dataBuffWrPos + dataLen;
    readCount = 0;
    while(dataBuffWrPos < dataBuffEndPos) {
        auto readCount = read(socket, dataBuffWrPos, dataBuffEndPos - dataBuffWrPos);
        if(readCount < 0) {
            throw string{ "Cannot read data" };
        }
        dataBuffWrPos += readCount;
    }

    return { static_cast<size_t>(dataLen), std::move(dataBuff) };
}
