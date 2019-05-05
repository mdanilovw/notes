#ifndef _RESPONSE_HPP_
#define _RESPONSE_HPP_

#include "return_code.hpp"

/**
 * Defines core service response entity
 */
struct Response {
    /**
     * Constructor
     * 
     * @param code Return code
     * @param records User records
     * @param summary Text summary
     */
    Response(ReturnCode code, vector<Record> &&records = {}, string &&summary = {}): 
    code{ code }, 
    records{ std::forward<vector<Record>>(records) },
    summary{ std::forward<string>(summary) }
    {}

    /**
     * Get return code
     * 
     * @return Return code
     */
    ReturnCode getCode() const;

    /**
     * Get records
     * 
     * @return Records
     */
    vector<Record>& getRecords();

    /**
     * Get summary
     * 
     * @return Summary
     */
    const string& getSummary() const;

    private:
    ReturnCode code;
    vector<Record> records;
    string summary;
};

#endif
