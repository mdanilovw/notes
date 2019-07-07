#ifndef _CORE_SERVICE_HPP_
#define _CORE_SERVICE_HPP_

#include <atomic>
#include <condition_variable>
#include <future>
#include <queue>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

#include "core.hpp"
#include "core_action.hpp"
#include "response.hpp"

#include <netinet/in.h>
#include <sys/socket.h>

using std::async;
using std::atomic;
using std::condition_variable;
using std::future;
using std::mutex;
using std::pair;
using std::promise;
using std::shared_ptr;
using std::thread;
using std::unique_ptr;
using std::queue;

/**
 * Virtual base class for intermediate layers between application core and client code. 
 * Manages access to the application core
 */
struct CoreService {
    /**
     * Constructor
     * 
     * @param core Pointer to the application core
     */
    CoreService(const shared_ptr<Core> &core): core{ core } {}

    /**
     * Execute Core Action
     * 
     * @param action Core Action to be executed
     * @return Response future
     */
    virtual future<Response> execAction(unique_ptr<CoreAction>&& action) = 0;

    /**
     * Destructor
     */
    virtual ~CoreService() {}

    /**
     * Start accepting commands
     */
    virtual void start() = 0;

    /**
     * Stop accepting commands
     */
    virtual void stop() = 0;

    protected:
    // Pointer to the application core object
    shared_ptr<Core> core;
};

/**
 * Provides access to the core for client code running on local machine
 */
struct LocalCoreService: public CoreService {
    /**
     * Constructor
     * 
     * @param core Pointer to the application core
     */
    LocalCoreService(shared_ptr<Core> &core): CoreService{ core }, stopService{ false } {}

    /**
     * Execute Core Action
     * 
     * @param action Core Action to be executed
     * @return Response future
     */
    future<Response> execAction(unique_ptr<CoreAction>&& action) override;

    /**
     * Start accepting commands 
     */
    void start() override;

    /**
     * Stop accepting commands
     */ 
    void stop() override;

protected:
    // Indicates whether service should continue operating
    atomic<bool> stopService;

private:
    // Core Actions queue
    queue<unique_ptr<CoreAction>> actions;
    // Core Actions queue synchronization
    mutex actionsMutex;
    // Notification about incoming Core Actions
    condition_variable actionsCv;
    // Executes Core Actions in the queue asynchronously 
    thread execActionThread;

    /**
     *  Executes Core Actions in the queue
     */ 
    void execActionLoop();
};

/**
 * Should provide access to the core for client code over network
 */
struct NetworkCoreService: public LocalCoreService {
    /**
     * @brief NetworkCoreService constructor
     * @param core Pointer to the application core
     */
    NetworkCoreService(shared_ptr<Core> &core): 
        LocalCoreService{ core }, 
        port{ 8080 }, 
        connectionQueueSize{ 30 }
    {}

    void start() override;
    void stop() override;

private:
    static constexpr int SOCKET_OPTION{ 1 };
    // Server port
    int port;
    // Number of connections in queue until they are refused
    int connectionQueueSize;
    // File descriptor corresponding server socket
    int serverFd;
    // Socket address
    struct sockaddr_in socketAddr;
    // Queue of sockets awaiting processing
    queue<int> sockets;
    // Socket queue synchronization
    mutex socketMutex;
    // Notification about incoming sockets
    condition_variable socketCv;
    // Socket processing thread
    thread socketProcessingThread;

    /**
     * @brief readSocket Read data from socket
     * @param socket Socket
     * @return Pair: data length + ptr to the data
     */
    virtual pair<size_t, unique_ptr<char>> readSocket(int socket);

    /**
     * @brief Create server socket
     */
    virtual void createServerSocket();

    /**
     * @brief Accept client requests from network interface
     */
    virtual void acceptRequests();

    /**
     * @brief Process particular client request
     * @param socket Target socket
     */
    virtual void processRequest(int socket);

    /**
     * @brief processNextRequest Process next request in the queue
     */
    virtual void processNextRequest();
};

#endif

