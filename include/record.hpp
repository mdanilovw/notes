#ifndef _RECORD_HPP_
#define _RECORD_HPP_

#include <algorithm>
#include <ctime>
#include <fstream>

#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/gregorian/greg_serialize.hpp"
#include "boost/serialization/vector.hpp"

using std::string;
using std::vector;

/**
 * Defines user data entry
 */
class Record {
public:

    /**
     * Constructor
     *
     * @param text Text
     * @param tags Tags
     * @param cdate Creation date
     * @param mdate Modification date
     * @param deleted Deleted state
     */
    Record(string &&text, vector<string> &&tags): 
        text    { std::forward<string>(text)               }, 
        tags    { std::forward<vector<string>>(tags)       },
        cdate   { boost::gregorian::day_clock::local_day() },
        mdate   { boost::gregorian::day_clock::local_day() },
        deleted { false                                    }
        {}

    /**
     * Add tag to a record
     * 
     * @param tag Tag to be added
     */
    void addTag(string &&tag);

    /**
     * Check whether a record contains specified text
     * 
     * @param text Text
     * @return True if the record contains specified text
     */
    bool containsText(const string& text) const;

    /**
     * Delete tag from a record
     * 
     * @param tag Tag to be deleted
     */
    void deleteTag(const string& tag);

    /**
     * Get record creation date
     * 
     * @return Record creation date
     */
    const boost::gregorian::date& getCreationDate() const;

    /**
     * Get string representation of a record creation date
     * 
     * @return date string
     */
    string getCreationDateStr() const;

    /**
     * Get record Id
     * 
     * @return Record Id
     */
    int getId() const;

    /**
     * Get record modification date
     * 
     * @return Record modification date
     */
    const boost::gregorian::date& getModificationDate() const;

    /**
     * Get modification date string
     * 
     * @return date string
     */
    string getModificationDateStr() const;

    /**
     * Get record tags
     * 
     * @return Record tags
     */
    const vector<string>& getTags() const;

    /**
     * Get record text
     * 
     * @return Record text
     */
    const string& getText() const;

    /**
     * Get record deleted state
     * 
     * @return Record deleted state 
     */
    bool isDeleted() const;

    /**
     * Equality operator
     *
     * @param record Record
     * @return True if records are logically equal
     */
    bool operator==(const Record &record);

    /**
     * Set deleted marker for the record
     *
     * @param state Deleted state to be set
     */
    void setDeleted(bool state);

    /**
     * Set record Id
     * 
     * @param id Record Id
     */
    void setId(int id);

    /**
     * Set record text
     * 
     * @param text Record text
     */
    void setText(string &&text);

    /**
     * Check whether the record has all specified tags
     *
     * @param tags Record tags
     * @return True if the record has all specified tags
     */
    bool tagged(const vector<string>& tags) const;

    /**
     * Check whether the record has specified tag
     *
     * @param tag Record tag
     * @return True if the record has specified tag
     */
    bool tagged(const string &tag) const;

private:
    /**
     * Constructor for serialization purposes
     */
    Record(){};

    // Unique identifier
    int id;
    
    // Text
    string text;

    // Attached tags 
    vector<string> tags;

    // Creation date
    boost::gregorian::date cdate; 

    // Modification date
    boost::gregorian::date mdate;

    // Deleted state
    bool deleted;
    
friend class boost::serialization::access;

    /*
     * Serialize record to persistent storage
     * 
     * @param ar Storage 
     * @param version Version
     */
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & cdate;
        ar & mdate;
        ar & tags;
        ar & text;
        ar & deleted;
    }
};

#endif // RECORD
