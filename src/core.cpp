/**
 * Implementation of the Core class
 */

#include <fstream>
#include <sstream>
#include "crypto.hpp"
#include "core.hpp"

const string Core::DATA_FILE = "notes_data";

int Core::NEXT_RECORD_ID;

ReturnCode Core::setPassword(string &&password) {
    if(password.empty()) {
        return ReturnCode::INVALID_PASSWORD;
    }

    this->password = std::move(password);	
    encryption = true;
    return ReturnCode::OK;
}

ReturnCode Core::start() {
    try {
        return init();
    } 
    catch (CryptoPP::InvalidCiphertext e) {
        return ReturnCode::WRONG_PASSWORD;
    } catch(...) {
        // TODO: provide more details
        return ReturnCode::GENERIC_ERROR;
    }
}

ReturnCode Core::addRecord(Record &&record) {
    record.setId(NEXT_RECORD_ID++);
    records.emplace_back(std::move(record));

    // TODO: define when sync should really ocur
    return sync();
}

ReturnCode Core::updateRecord(Record &&record) {
    auto id = record.getId();
    //auto records = search([id](const Record& r){return r.getId() == id;});
    auto recordIter = std::find_if(records.begin(), records.end(), 
        [id](const Record& r){return r.getId() == id;});

    if(recordIter != records.end()) {
        std::swap(*recordIter, record);
        sync();
        return ReturnCode::OK;
    }
    
    return ReturnCode::NOT_FOUND;
}

ReturnCode Core::removeRecord(int id) {
    records.erase(
        std::remove_if(
            records.begin(), records.end(), [id](const Record &r){return r.getId() == id;}),
        records.end());

    return sync();
}

ReturnCode Core::sync() {
    std::ofstream ofs(DATA_FILE);
    if(!ofs.is_open()) throw "I/O ERROR";

    if(encryption) {
        std::stringstream textStream;
        boost::archive::text_oarchive oa(textStream);
        oa << records;
        Crypto crt(password);
        string encryptedData = crt.encryptString(textStream.str());
        ofs << encryptedData;
    } else {
        boost::archive::text_oarchive oa(ofs); 
        oa << records;
    }

    return ReturnCode::OK;
}

vector<Record> Core::search(const RecordPredicate &pred) {
    vector<Record> recordsFound;
    for(auto &record : records) {
        if(pred(record)) recordsFound.push_back(record);
    }

    return recordsFound;
}

ReturnCode Core::init() {
    std::ifstream ifs(DATA_FILE);

    if(!ifs.is_open()) return ReturnCode::EMPTY;
    
    if(encryption) {
        std::stringstream encrypted_data;
        encrypted_data << ifs.rdbuf();
	
        Crypto crt(password);
        std::stringstream decrypted_data(crt.decryptString(encrypted_data.str()));
        boost::archive::text_iarchive ia(decrypted_data);
        ia >> records;
    } else {
        boost::archive::text_iarchive ia(ifs);
        ia >> records;
    }

    for(auto &record : records) {
        record.setId(NEXT_RECORD_ID++);
    }

    return ReturnCode::OK;
}