#ifndef _CORE_HPP_
#define _CORE_HPP_

#include <vector>
#include "record.hpp"
#include "return_code.hpp"

using std::string;
using std::vector;

using RecordPredicate = std::function<bool (const Record&)>;

/**
 * Provides reading/writing/searching/ of user data
 */
class Core {
public:
    /**
     * Add a new record
     *
     * @param record Record
     * @return OK - if record was successfully added
     *         May throw I/O exception
     */
    ReturnCode addRecord(Record &&record);

    /**
     * Update a record
     * 
     * @param record Record with updated content
     * @return OK        - if record was successfully updated 
     *         NOT_FOUND - if required record does not exit
     */
    ReturnCode updateRecord(Record &&record);

    /**
     * Read user data from persistent storage
     * 
     * @return OK    - if data initialization completed successfully
     *         EMPTY - if no data present in persistent storage
     *         May throw I/O exception
     */
    ReturnCode init();

    /**
     * Remove record
     *
     * @param recordId Id of the Record to be removed
     * @return OK - if record was successfully removed
     *         May throw I/O exception
     */
    ReturnCode removeRecord(int recordId);

    /**
     * Search records
     *
     * @param pred Record predicate
     * @return Copies of records found
     */
    vector<Record> search(const RecordPredicate &pred);

    /**
     * Set user password for data encryption/decryption
     *
     * @param password Current user password
     * @return OK               - successfull result
     *         INVALID_PASSWORD - password does not match expected format
     */
    ReturnCode setPassword(string &&password);

    /**
     * Initialize core with user data
     * 
     * @return OK             - User data is initialized successfully 
     *         WRONG_PASSWORD - Not able to decrypt data with specified password
     *         GENERIC_ERROR  - Unexpected error
     *         May throw I/O exception
     */
    ReturnCode start();

    /**
     * Write user data to persistent storage
     * 
     * @return OK - if user data was successfully saved
     *         May throw I/O exception
     */
    ReturnCode sync();
	
private:
    static const string DATA_FILE;
    static int NEXT_RECORD_ID;

    string password;
    bool encryption;
    vector<Record> records;
};

#endif // CORE
