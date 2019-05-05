#include "record.hpp"
#include "response.hpp"

vector<Record>& Response::getRecords() {
    return records;
}

ReturnCode Response::getCode() const {
    return code;
}

const string& Response::getSummary() const {
    return summary;
}