#ifndef _CLI_HPP_
#define _CLI_HPP_

#include <atomic>
#include <map>
#include <memory>
#include "core_service.hpp"
#include "return_code.hpp"
#include "util.hpp"

using std::atomic;
using std::map;
using std::string;
using std::thread;
using std::vector;
using std::shared_ptr;

/**
 * Defines CLI command
 */
struct CliCommand {
    /**
     * Constructor
     * 
     * @param cmd Command
     * @param args Command arguments
     */
    CliCommand(string &&cmd = {}, map<string, string> &&args = {}):
    cmd{ std::forward<string>(cmd) },
    args{ std::forward<map<string, string>>(args) }
    {}

    /**
     * Assignment operator
     * 
     * @param other CliCommand
     * @return Reference to the result object 
     */
    CliCommand& operator=(const CliCommand &other);

    /**
     * Equality operator
     * 
     * @cmd String command
     * @return True if command is logically equal to the string command provided
     */
    bool operator==(const string &cmd) const;

    /**
     * Inequality operator
     * 
     * @cmd String command
     * @return True if command is not logically equal to the string command provided 
     */
    bool operator!=(const string &cmd) const;

    /**
     * Check whether command has particular argument
     * 
     * @param arg Argument
     * @return True if specified argument is present
     */
    bool hasArgument(const string &arg) const;

    /**
     * Get argument value
     * 
     * @param arg Argument
     * @return Argument value
     */
    const string& getArgument(const string &arg) const;

    /**
     * Get list of argument values (if multiple values defined and separated by spaces)
     * 
     * @param arg Argument
     * @return Argument values
     */
    const vector<string> getArgumentList(const string &arg) const;

    /**
     * Get command
     * 
     * @return Command
     */
    const string& get() const;

    private:
    string cmd;
    map<string, string> args;
};

/**
 * Provides application Command Line Interface
 */
class Cli {
public:
    static constexpr auto NO_ENCRYPTION = "-no-encryption";

    /**
     * Constructor
     *
     * @param core Application core
     * @param encryption Specifies encryption mode on/off
     */
    Cli(shared_ptr<CoreService> &coreService, bool encryption): 
        coreService{ coreService }, 
        encryption{ encryption },
        stop{ false }
        {}

    /**
     * Start command line interface
     * @return OK               - if completed successfully
     *         INVALID_PASSWORD - if specified password has invalid format
     *         WRONG_PASSWORD   - if specified password is incorrect
     *         GENERIC_ERROR    - for any other error condition
     */
    ReturnCode start();

private:
    // Commands
    static const string ADD_CMD;
    static const string EXIT_CMD;
    static const string FIND_CMD;
    static const string IMPORT_CMD;
    static const string ADD_TAG_CMD;
    static const string DEL_RECORD_CMD;
    static const string DEL_TAG_CMD;
    static const string EDIT_RECORD_CMD;
    static const string NEXT_RECORD_CMD;
    static const string PREV_RECORD_CMD;
    static const string QUIT_SEARCH_CMD;

    // Command arguments (options)
    static const string CDATE_AFTER_OPT;
    static const string CDATE_BEFORE_OPT;
    static const string DELETED_OPT;
    static const string FRAGMENT_OPT;
    static const string MDATE_AFTER_OPT;
    static const string MDATE_BEFORE_OPT;
    static const string TAG_OPT;
    static const string TAGS_OPT;

    // Messages
    static const string MSG_SERVER_NOT_RESPONDED;
    static const string MSG_INVALID_PASSWD;
    static const string MSG_UNKNOWN_ERROR;
    static const string MSG_TAG_NOT_ADDED;
    static const string MSG_TAG_NOT_DELETED;
    static const string MSG_RECORD_NOT_ADDED;
    static const string MSG_RECORDS_NOT_FOUND;
    static const string MSG_EDIT_FAILED;
    static const string MSG_DATE_FORMAT_ERROR;
    static const string MSG_UNKNOWN_COMMAND;
    static const string MSG_DELETE_FAILED;
    static const string INPUT_TAG_PROMPT;
    static const string INPUT_PASSWORD_PROMPT;

    static constexpr int CORE_SERVICE_RESPONSE_TIMEOUT{ 30 };
    static const string SCROLL_MENU;
    static const vector<string> KEYWORDS;
    static const string TMP_FILE_PATH;
    static const string OPEN_TMP_FILE;
    static const string RM_TMP_FILE;
    static const string PROMPT;
    static const string DELETED_MARKER;

    // Pointer to Core Service the CLI should operate with
    shared_ptr<CoreService> coreService;
    // Encryption mode on/off
    bool encryption;
    // Indicates whether CLI should continue operating
    atomic<bool> stop;

    /**
     *  Read(user input) -> Eval loop
     */
    void readEvalLoop();

    /** 
     * Add a new record
     *
     * @param cmd User command
     * 
     * @return OK            - if record was successfully added
     *         GENERIC_ERROR - if record was not added
     */
    ReturnCode addRecord(const CliCommand &cmd) const; 

    /** 
     * Add a new tag to the existing record
     *
     * @param record Record the tag should be added to
     * @return OK            - if tag was successfully added
     *         GENERIC_ERROR - if tags was not added
     */
    ReturnCode addTag(Record &record) const;

    /**
     * Translate record searching command into predicate accepting record
     *
     * @param cmd User command
     * @return Record predicate which reflects search conditions provided in the user command
     */
     RecordPredicate generateRecordPredicate(const CliCommand &cmd) const;

    /** 
     * Delete tag form existing record
     *
     * @param record Record the tag should be removed from
     * @return OK            - if tag was successfully deleted
     *         GENERIC_ERROR - if tag was not deleted
     */
    ReturnCode deleteTag(Record &record) const;

    /** 
     * Dispatch user command
     *
     * @param cmd User command
     * @return Return code of appropriate handler function
     */
    ReturnCode dispatchCmd(const CliCommand &cmd);
    
    /** 
     * Edit existing record
     *
     * @param record Record that should be edited
     * @return OK            - if record editing completed successfully
     *         GENERIC_ERROR - if record editing failed
     */
    ReturnCode editRecord(Record &record) const;

    /**
     * Delete record
     * 
     * @param record Record to be deleted
     * @return OK            - if record was successfully deleted
     *         GENERIC_ERROr - if record was not deleted
     */
    ReturnCode deleteRecord(Record &record) const;

    /** 
     * Find records and print result
     *
     * @param cmd User command
     * @return Return code of call to scrollRecords
     *         NOT_FOUND - if records were not found
     */
    ReturnCode searchRecords(const CliCommand &cmd) const;

    /** 
     * Parse string user command into CliCommand object
     *
     * @param strCommand String command
     * @return CliCommand object
     */
    CliCommand parseCommand(string &strCommand) const;

    /** 
     * Display record on the screen
     *
     * @param record Record to be displayed
     */
    void displayRecord(const Record &record) const;

    /** 
     * Prompt user password
     * 
     * @return Password
     */
    string requestPassword(); 

    /**
     * Display multiple records sequentially on a screen
     *
     * @param records Records to be displayed
     * @return OK            - if operation completed successfully
     *         GENERIC_ERROR - if operation failed
     */
    ReturnCode scrollRecords(vector<Record> &records) const;

    /**
     * Exit from command line interface
     * 
     * @param cmd User command
     * @return OK
     */
    ReturnCode exitCli(const CliCommand &cmd);

    /**
     * Open external text editor for record text modification
     * 
     * @param text Record text supposed for modification
     * @return Edited record text
     */
    string openEditor(const string &text) const;

    /**
     * Display message on the screen
     * 
     * @param msg Message
     */
    void message(const string &msg) const;

    /**
     * Get user input
     * 
     * @param prompt Message to be displayed on the screen
     * @return User response
     */
    string prompt(const string &prompt) const;
};

#endif
