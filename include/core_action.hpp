#ifndef _CORE_ACTION_HPP_
#define _CORE_ACTION_HPP_

#include <memory>
#include <future>
#include "core.hpp"
#include "response.hpp"

using std::future;
using std::shared_ptr;

/**
 * Virtual base class for entities which define particular operations (and context data)
 * to the application core
 */
struct CoreAction {
    /**
     * Execute the Action
     */
    virtual void exec() = 0;

    /**
     * Undo the Action
     */
    virtual void undo() = 0;

    /**
     * Set pointer to the application core
     * 
     * @param core Pointer to the application core
     */
    virtual void setCore(const shared_ptr<Core> &core);

    /**
     * Get future for retrieving execution response
     * 
     * @return Response future
     */
    virtual future<Response> getFuture();

    /**
     * Destructor
     */
    virtual ~CoreAction() {};

    protected:
    std::shared_ptr<Core> core;
    std::promise<Response> responsePromise;
};

/**
 * Add a new record
 */
struct AddRecordAction: public CoreAction {
    /**
     * Constructor
     * 
     * @param text Record text
     * @param tags Record tags
     */
    AddRecordAction(string &&text, vector<string> &&tags): 
    text{ std::forward<string>(text) },
    tags{ std::forward<vector<string>>(tags) }
    {}

    /**
     * Adds a new record
     */
    void exec() override;

    /**
     * Undo adding a new record
     */
    void undo() override;

    private:
    string text;
    vector<string> tags;
};

/**
 * Update a record
 */
struct UpdateRecordAction: public CoreAction {
    /**
     * Constructor
     * 
     * @param record Record to be updated
     */
    UpdateRecordAction(Record &&record): record{ std::forward<Record>(record) } {}

    /**
     * Substitutes a record with same Id by the record specified
     */
    void exec() override;

    /**
     * Undo updating record
     */
    void undo() override;

    private:
    Record record;
};

/**
 * Search for the records
 */
struct SearchRecordsAction: public CoreAction {
    /**
     * Constructor
     * 
     * @param pred Record predicate
     */
    SearchRecordsAction(RecordPredicate &&pred): pred{ std::forward<RecordPredicate>(pred) } {}

    /**
     * Searches for the record using specified predicate
     */
    void exec() override;

    /**
     * Undo record searching
     */
    void undo() override;

    private:
    RecordPredicate pred;
};

/**
 * Set password for data encryption/decryption
 */
struct SetPasswordAction: public CoreAction {
    /**
     * Constructor
     * 
     * @param password Password
     */
    SetPasswordAction(string &&password): password{ std::forward<string>(password) } {}

    /**
     * Sets encryption/decryption password
     */
    void exec() override;

    /**
     * Undo setting password
     */
    void undo() override;

    private:
    string password;
};

/**
 * Start the application core
 */
struct StartAction: public CoreAction {
    /**
     * Starts the application core
     */
    void exec() override;

    /**
     * Undo starting the application core
     */
    void undo() override;
};

#endif
