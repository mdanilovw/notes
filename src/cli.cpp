/**
 * Cli implementation
 */

#include "boost/algorithm/string.hpp"
#include <functional>
#include <iostream>
#include <thread>
#include "cli.hpp"
#include "record.hpp"

#define ready std::future_status::ready 

using boost::gregorian::date;
using std::cin;
using std::cout;
using std::endl;
using std::pair;
using std::string;
using std::vector;
using std::string;
using std::thread;
using std::unique_ptr;

const string Cli::ADD_CMD                  { "add" };
const string Cli::EXIT_CMD                 { "exit" };
const string Cli::FIND_CMD                 { "find" };
const string Cli::ADD_TAG_CMD              { "at" };
const string Cli::DEL_RECORD_CMD           { "delete" };
const string Cli::DEL_TAG_CMD              { "dt" };
const string Cli::EDIT_RECORD_CMD          { "e" };
const string Cli::NEXT_RECORD_CMD          { "n" };
const string Cli::PREV_RECORD_CMD          { "p" };
const string Cli::QUIT_SEARCH_CMD          { "q" };


const string Cli::CDATE_AFTER_OPT          { "-after" };
const string Cli::CDATE_BEFORE_OPT         { "-before" };
const string Cli::DELETED_OPT              { "-deleted" };
const string Cli::FRAGMENT_OPT             { "-txt" };
const string Cli::MDATE_AFTER_OPT          { "-mafter" };
const string Cli::MDATE_BEFORE_OPT         { "-mbefore" };
const string Cli::TAG_OPT                  { "-tag" };
const string Cli::TAGS_OPT                 { "-tags" };

const string Cli::TMP_FILE_PATH            { "/var/tmp/notes" };
const string Cli::OPEN_TMP_FILE            { "vi " + Cli::TMP_FILE_PATH };
const string Cli::RM_TMP_FILE              { "rm " + Cli::TMP_FILE_PATH };
const string Cli::PROMPT                   { "> " };
const string Cli::DELETED_MARKER           { "<___ DELETED ___>" };

const string Cli::INPUT_PASSWORD_PROMPT    { "Enter Password\n" };
const string Cli::INPUT_TAG_PROMPT         { "Tag: " };

const string Cli::MSG_SERVER_NOT_RESPONDED { "Server has not responded\n" };
const string Cli::MSG_INVALID_PASSWD       { "Invalid password\n" };
const string Cli::MSG_UNKNOWN_ERROR        { "Unknown error\n" };
const string Cli::MSG_TAG_NOT_ADDED        { "Unable to add Tag\n" };
const string Cli::MSG_TAG_NOT_DELETED      { "Unable to delete Tag\n" };
const string Cli::MSG_RECORD_NOT_ADDED     { "Unable to add record\n" };
const string Cli::MSG_RECORDS_NOT_FOUND    { "Records not found for the given request\n" };
const string Cli::MSG_EDIT_FAILED          { "Record editing has failed\n" };
const string Cli::MSG_DATE_FORMAT_ERROR    { "Unexpected date format. Use YYYY-MM-DD\n" };
const string Cli::MSG_UNKNOWN_COMMAND      { "Unknown command\n" };
const string Cli::MSG_DELETE_FAILED        { "Unable to delete record" };

const string Cli::SCROLL_MENU {
    "[" + NEXT_RECORD_CMD + " - Next] "        +
    "[" + PREV_RECORD_CMD + " - Prev] "        +
    "[" + EDIT_RECORD_CMD + " - Edit] "        +
    "[" + ADD_TAG_CMD     + " - Add Tag] "     +
    "[" + DEL_TAG_CMD     + " - Del Tag] "     +
    "[" + DEL_RECORD_CMD  + " - Del Rec] "     +
    "[" + QUIT_SEARCH_CMD + " - Quit Search] "
}; 

constexpr int Cli::CORE_SERVICE_RESPONSE_TIMEOUT;

const vector<string> Cli::KEYWORDS {
        ADD_CMD, CDATE_AFTER_OPT, CDATE_BEFORE_OPT, DELETED_OPT, FIND_CMD, 
        FRAGMENT_OPT, MDATE_AFTER_OPT, MDATE_BEFORE_OPT, TAG_OPT, TAGS_OPT
};

//struct termios Cli::SAVED_TERM_ATTR;

ReturnCode Cli::addRecord(const CliCommand &cmd) const {
    auto text = openEditor("");
    auto tags = cmd.getArgumentList(TAG_OPT);
    unique_ptr<CoreAction> addRecordAction{ new AddRecordAction{ std::move(text), std::move(tags) } };
    auto futureResponse = coreService->execAction(std::move(addRecordAction));
    auto responseStatus = futureResponse.wait_for(
        std::chrono::seconds(CORE_SERVICE_RESPONSE_TIMEOUT));

    if(responseStatus != ready || futureResponse.get().getCode() != ReturnCode::OK) {
        message(MSG_RECORD_NOT_ADDED);
        return ReturnCode::GENERIC_ERROR;
    }
    
    return ReturnCode::OK;
}

ReturnCode Cli::addTag(Record &record) const {
    auto tag = prompt(INPUT_TAG_PROMPT);
    record.addTag(std::move(tag));
    unique_ptr<CoreAction> updateRecordAction{ new UpdateRecordAction{ Record{record} } };
    auto futureResponse = coreService->execAction(std::move(updateRecordAction));
    auto responseStatus = futureResponse.wait_for(
        std::chrono::seconds(CORE_SERVICE_RESPONSE_TIMEOUT));

    if(responseStatus != ready || futureResponse.get().getCode() != ReturnCode::OK) {
        message(MSG_TAG_NOT_ADDED);
        return ReturnCode::GENERIC_ERROR;
    }

    return ReturnCode::OK;
}

RecordPredicate Cli::generateRecordPredicate(const CliCommand &cmd) const {
    // Initialize filter to pick non-deleted records
    RecordPredicate pred = [](const Record &rec){return !rec.isDeleted();};

    // Search by deleted state
    if(cmd.hasArgument(DELETED_OPT)) {
        pred = Util::INV<Record>(pred);
    }

    // Search by multiple tags
    if(cmd.hasArgument(TAGS_OPT)) {
        auto tags = cmd.getArgumentList(TAGS_OPT);
        pred = Util::AND<Record>(pred, [tags] (const Record &rec) {
            return rec.tagged(tags);
        });
    }

    // Search by single tag (record contains at least one of specified tags)
    if(cmd.hasArgument(TAG_OPT)) {
        auto tags = cmd.getArgumentList(TAG_OPT);
        pred = Util::AND<Record>(pred, [tags] (const Record &rec) {
            for(const string &tag : tags) {
                if(rec.tagged(tag)) return true;
            }
            return false;
        });
    }

    // Search by creation date (pick older records)
    if(cmd.hasArgument(CDATE_BEFORE_OPT)) {
        try {
            date recordDate(boost::gregorian::from_string(cmd.getArgument(CDATE_BEFORE_OPT)));
            pred = Util::AND<Record>(pred, [recordDate](const Record &rec) {
                return rec.getCreationDate() < recordDate;
            });
        } catch(...) {
            message(MSG_DATE_FORMAT_ERROR);
            return [](const Record&){return false;};
        }
    }

    // Search by creation date (pick newer records)
    if(cmd.hasArgument(CDATE_AFTER_OPT)) {
        try {
            date recordDate(boost::gregorian::from_string(cmd.getArgument(CDATE_AFTER_OPT)));
            pred = Util::AND<Record>(pred, [recordDate](const Record &rec) {
                return rec.getCreationDate() >= recordDate;
            });
        } catch(...) {
            message(MSG_DATE_FORMAT_ERROR);
            return [](const Record&){return false;};
        }
    }

    // Search by modification date (pick older records)
    if(cmd.hasArgument(MDATE_BEFORE_OPT)) {
        try {
            date recordDate(boost::gregorian::from_string(cmd.getArgument(MDATE_BEFORE_OPT)));
            pred = Util::AND<Record>(pred, [recordDate](const Record &rec) {
                return rec.getModificationDate() < recordDate;
            });
        } catch(...) {
            message(MSG_DATE_FORMAT_ERROR);
            return [](const Record&){return false;};
        }
    }

    // Search by modification date (pick newer records)
    if(cmd.hasArgument(MDATE_AFTER_OPT)) {
        try {
            date recordDate(boost::gregorian::from_string(cmd.getArgument(MDATE_AFTER_OPT)));
            pred = Util::AND<Record>(pred, [recordDate](const Record &rec) {
                return rec.getModificationDate() >= recordDate;
            });
        } catch(...) {
            message(MSG_DATE_FORMAT_ERROR);
            return [](const Record&){return false;};
        }
    }
    
    // Search by text fragment
    if(cmd.hasArgument(FRAGMENT_OPT)) {
        const auto &text = cmd.getArgument(FRAGMENT_OPT);
        pred = Util::AND<Record>(pred, [text] (const Record &rec) {
            return rec.containsText(text);
        });
    }

    return pred;
}

ReturnCode Cli::deleteTag(Record &record) const {
    auto tag = prompt(INPUT_TAG_PROMPT);
    record.deleteTag(tag);
    unique_ptr<CoreAction> updateRecordAction{ new UpdateRecordAction{ Record{record} } };
    auto futureResponse = coreService->execAction(std::move(updateRecordAction));
    auto responseStatus = futureResponse.wait_for(
        std::chrono::seconds(CORE_SERVICE_RESPONSE_TIMEOUT));

    if(responseStatus != ready || futureResponse.get().getCode() != ReturnCode::OK) {
        message(MSG_TAG_NOT_DELETED);
        return ReturnCode::GENERIC_ERROR;
    }

    return ReturnCode::OK;
}

ReturnCode Cli::dispatchCmd(const CliCommand &cmd) {
    if(cmd == "") 
        return ReturnCode::OK;
    if(cmd == ADD_CMD)  
        return addRecord(cmd);
    if(cmd == FIND_CMD) 
        return searchRecords(cmd);
    if(cmd == EXIT_CMD) 
        return exitCli(cmd);
    else 
        message(MSG_UNKNOWN_COMMAND);

    return ReturnCode::GENERIC_ERROR;
}

ReturnCode Cli::editRecord(Record &record) const {
    auto new_text = openEditor(record.getText());
    record.setText(std::move(new_text));
    unique_ptr<CoreAction> updateRecordAction{ new UpdateRecordAction{ Record{ record } } };
    auto futureResponse = coreService->execAction(std::move(updateRecordAction));
    auto responseStatus = futureResponse.wait_for(
        std::chrono::seconds(CORE_SERVICE_RESPONSE_TIMEOUT));

    if(responseStatus != ready || futureResponse.get().getCode() != ReturnCode::OK) {
        message(MSG_EDIT_FAILED);
        return ReturnCode::GENERIC_ERROR;
    }

    return ReturnCode::OK;
}

ReturnCode Cli::deleteRecord(Record &record) const {
    record.setDeleted(true);
    unique_ptr<CoreAction> updateRecordAction{ new UpdateRecordAction{ Record{ record } } };
    auto futureResponse = coreService->execAction(std::move(updateRecordAction));
    auto responseStatus = futureResponse.wait_for(
        std::chrono::seconds(CORE_SERVICE_RESPONSE_TIMEOUT));

    if(responseStatus != ready || futureResponse.get().getCode() != ReturnCode::OK) {
        message(MSG_DELETE_FAILED);
        return ReturnCode::GENERIC_ERROR;
    }

    return ReturnCode::OK;
}

ReturnCode Cli::searchRecords(const CliCommand &cmd) const {
    auto pred = generateRecordPredicate(cmd);
    unique_ptr<CoreAction> searchRecordsAction{ new SearchRecordsAction{ std::move(pred) } };
    auto responseFuture = coreService->execAction(std::move(searchRecordsAction));
    auto status = responseFuture.wait_for(std::chrono::seconds(CORE_SERVICE_RESPONSE_TIMEOUT));

    if(status == ready) {
        return scrollRecords(responseFuture.get().getRecords());
    } else {
        message(MSG_RECORDS_NOT_FOUND);
        return ReturnCode::NOT_FOUND;
    }
}

CliCommand Cli::parseCommand(string &strCommand) const {
    string cmd;
    string args;
    auto cmdEndPos = strCommand.find(" ");
    if(cmdEndPos != string::npos) {
        cmd = strCommand.substr(0, cmdEndPos);
        args = strCommand.substr(cmdEndPos);
    } else {
        return CliCommand{ std::move(strCommand), {} };
    }

    // Parse command arguments
    vector<string> tokens;
    boost::split(tokens, args, boost::is_any_of("\t "));

    string keyToken;
    string valueToken;
    map<string, string> argsParsed;

    auto storeTokens = [&argsParsed](string &&keyToken, string &&valueToken) {
        boost::trim(valueToken);
        argsParsed.emplace(std::make_pair(std::forward<string>(keyToken), 
            std::forward<string>(valueToken)));
    };

    for(auto &token : tokens) {
        if(std::find(KEYWORDS.begin(), KEYWORDS.end(), token) != KEYWORDS.end()) {
            if(keyToken != "") 
                storeTokens(std::move(keyToken), std::move(valueToken));

            valueToken = "";
            keyToken = token;
        } else {
            valueToken += token + " ";
        }
    }

    if(keyToken != "") 
        storeTokens(std::move(keyToken), std::move(valueToken));

    return CliCommand{ std::move(cmd), std::move(argsParsed) };
}

void Cli::displayRecord(const Record &record) const {
    // Tags
    for(const auto &tag : record.getTags())
        cout << tag << " | ";
    cout << endl;

    // Deleted marker
    if(record.isDeleted())
        cout << DELETED_MARKER << endl << endl;

    // Body
    cout << record.getText() << endl << endl << endl;

    // Date
    cout << record.getCreationDateStr() << " / " << record.getModificationDateStr() << endl;
}

string Cli::requestPassword() {
    message(INPUT_PASSWORD_PROMPT);
    Util::SetTerminalPasswordInputMode();

    string password;
    cin >> password;
    Util::SetTerminalNormalInputMode();
    return password;
}

ReturnCode Cli::start() {
    if(encryption) {
        unique_ptr<CoreAction> setPasswdAction { new SetPasswordAction{ requestPassword() } };
        auto responseFuture = coreService->execAction(std::move(setPasswdAction));
        auto responseStatus = responseFuture.wait_for(
            std::chrono::seconds(Cli::CORE_SERVICE_RESPONSE_TIMEOUT));

        if(responseStatus != ready) {
            message(MSG_SERVER_NOT_RESPONDED);
            return ReturnCode::GENERIC_ERROR;
        }

        if(responseFuture.get().getCode() == ReturnCode::INVALID_PASSWORD) {
            message(MSG_INVALID_PASSWD);
            return ReturnCode::INVALID_PASSWORD;
        }
    }

    unique_ptr<CoreAction> startAction { new StartAction() };
    auto futureResponse = coreService->execAction(std::move(startAction));
    auto responseStatus = futureResponse.wait_for(
        std::chrono::seconds(CORE_SERVICE_RESPONSE_TIMEOUT));
    if(responseStatus != ready) {
        message(MSG_SERVER_NOT_RESPONDED);
        return ReturnCode::GENERIC_ERROR;
    }

    auto code = futureResponse.get().getCode();
    if(code == ReturnCode::WRONG_PASSWORD) {
        message(MSG_INVALID_PASSWD);
        return code;
    }

    if(code == ReturnCode::GENERIC_ERROR) {
        message(MSG_UNKNOWN_ERROR);
        return code;
    }

   readEvalLoop();
}

ReturnCode Cli::scrollRecords(vector<Record> &records) const {
    auto recordIdx{ 0 };
    const auto recordNum = records.size();

    while(recordIdx < recordNum) {
        cout << recordIdx + 1 << "(" << recordNum << ")" << endl << endl;
        displayRecord(records[recordIdx]);
        CliCommand cmd{ prompt(SCROLL_MENU), {} };

        if(cmd == DEL_RECORD_CMD && deleteRecord(records[recordIdx++]) != ReturnCode::OK) 
            return ReturnCode::GENERIC_ERROR;

        else if(cmd == EDIT_RECORD_CMD && editRecord(records[recordIdx]) != ReturnCode::OK)
            return ReturnCode::GENERIC_ERROR;

        else if(cmd == DEL_TAG_CMD && deleteTag(records[recordIdx]) != ReturnCode::OK)
            return ReturnCode::GENERIC_ERROR;

        else if(cmd == ADD_TAG_CMD && addTag(records[recordIdx]) != ReturnCode::OK)
            return ReturnCode::GENERIC_ERROR;

        else if(cmd == PREV_RECORD_CMD && recordIdx > 0)
            recordIdx--;

        else if(cmd == NEXT_RECORD_CMD)
            recordIdx++;

        else if(cmd == QUIT_SEARCH_CMD)
            break;
    }

    return ReturnCode::OK;
}

ReturnCode Cli::exitCli(const CliCommand &cmd) {
    stop.store(true);
    return ReturnCode::OK;
}

string Cli::openEditor(const string &text) const {
    // Remove tmp file
    std::remove(TMP_FILE_PATH.c_str());

    // Write data to the tmp file
    std::ofstream ofs{ TMP_FILE_PATH };
    if (ofs.is_open()) {
        ofs << text;
        ofs.close();
    } else {
        // TODO: log error
        return "";
    }

    // Open tmp file in editor
    system(OPEN_TMP_FILE.c_str());

    // Retrieve edited text
    std::ifstream ifs(TMP_FILE_PATH);
    if(ifs.is_open()) {
        string new_text((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        ifs.close();
        return new_text;
    } else {
        // TODO: log error
        return "";
    }
}

void Cli::message(const string& msg) const {
    cout << msg;
}

string Cli::prompt(const string &prompt) const {
    message(prompt);
    string response;
    getline(cin, response);
    return response;
}

void Cli::readEvalLoop() {
    while(!stop) {
        auto userInput = prompt(PROMPT);
        dispatchCmd(parseCommand(userInput));
    }
}

CliCommand& CliCommand::operator=(const CliCommand &other) {
    cmd = other.cmd;
    args = other.args;
    return *this;
}

bool CliCommand::operator==(const string &str) const {
    return cmd == str;
}

bool CliCommand::operator!=(const string &str) const {
    return cmd != str;
}

bool CliCommand::hasArgument(const string &arg) const {
    return args.find(arg) != args.end();
}

const string& CliCommand::getArgument(const string &arg) const {
    return args.find(arg)->second;
}

const vector<string> CliCommand::getArgumentList(const string &arg) const {
    if(!hasArgument(arg)) {
        return {};
    }

    auto value = getArgument(arg);
    vector<string> valueList;

    if(value != "") {
        boost::split(valueList, value, boost::is_any_of("\t "));
    }

    return valueList;
}

const string& CliCommand::get() const {
    return cmd;
}