#ifndef _CORE_SERVICE_HPP_
#define _CORE_SERVICE_HPP_

#include <atomic>
#include <condition_variable>
#include <future>
#include <queue>
#include <memory>
#include <mutex>
#include <thread>

#include "core.hpp"
#include "core_action.hpp"
#include "response.hpp"

using std::async;
using std::atomic;
using std::condition_variable;
using std::future;
using std::mutex;
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
    virtual ~CoreService() {};

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

    private:
    // Core Actions queue
    queue<unique_ptr<CoreAction>> actions;
    // Core Actions queue synchronization
    mutex actionsQueueMutex;
    // Notification about incoming Core Actions
    condition_variable actionsQueueCv;
    // Indicates whether service should continue operating
    atomic<bool> stopService;
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
struct NetworkCoreService: public CoreService {
    future<Response> execAction(unique_ptr<CoreAction>&&) override {
        throw string{ "Network Core Service is not implemented" };
    }
};

#endif

