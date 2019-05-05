#include "core_service.hpp"

#include <condition_variable>
#include <mutex>
#include <thread>

using std::condition_variable;
using std::mutex;
using std::thread;
using std::unique_lock;

future<Response> LocalCoreService::execAction(unique_ptr<CoreAction> &&action) {
    future<Response> fut = action->getFuture();
    action->setCore(core);
    unique_lock<mutex> actionsQueueLock(actionsQueueMutex);
    actions.push(std::move(action));
    actionsQueueLock.unlock();
    actionsQueueCv.notify_one();
    return fut;
}

void LocalCoreService::execActionLoop() {
    while(!stopService.load()) {
        unique_lock<mutex> actionsQueueLock(actionsQueueMutex);
        if(!actions.empty()) {
            actions.front()->exec();
            actions.pop();
        } else {
            actionsQueueCv.wait(actionsQueueLock);
        }
        actionsQueueLock.unlock();
    }
}

void LocalCoreService::start()  {
    execActionThread = thread{ &LocalCoreService::execActionLoop, this };
}

void LocalCoreService::stop() {
    stopService.store(true);
    actionsQueueCv.notify_one();
    execActionThread.join();
}