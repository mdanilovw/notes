/**
 * Record class implementation
 */

#include "record.hpp"

void Record::addTag(string &&tag) {
    if(!tagged(tag)) {
        tags.push_back(std::move(tag));
        mdate = boost::gregorian::day_clock::local_day();
    }
}

bool Record::containsText(const string &text) const {
    auto recordTxt = this->text;
    auto requiredTxt = text;

    boost::algorithm::to_lower(recordTxt);
    boost::algorithm::to_lower(requiredTxt);

    if(boost::algorithm::contains(recordTxt, requiredTxt))
        return true;

    return false;
}

void Record::deleteTag(const string &tag) {
    tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());
    mdate = boost::gregorian::day_clock::local_day();
}

const boost::gregorian::date& Record::getCreationDate() const {
    return cdate;
}

string Record::getCreationDateStr() const {
    return boost::gregorian::to_simple_string(cdate);
}

const boost::gregorian::date& Record::getModificationDate() const {
    return mdate;
}

string Record::getModificationDateStr() const {
    return boost::gregorian::to_simple_string(mdate);
}

const vector<string>& Record::getTags() const {
    return tags;
}

const string& Record::getText() const {
    return text;
}

bool Record::isDeleted() const {
    return deleted;
}

bool Record::operator==(const Record &record) {
    return text == record.getText();
}

void Record::setDeleted(bool state) {
    deleted = state;
}

void Record::setText(string &&newText) {
    text = std::forward<string>(newText);
    mdate = boost::gregorian::day_clock::local_day();
}

bool Record::tagged(const vector<string> &tags) const {
    if(this->tags.size() < tags.size()) return false;

    for(const auto &tag : tags) {
        if(!tagged(tag)) return false;
    }

    return true;
}

bool Record::tagged(const string &tag) const {
    return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

int Record::getId() const {
    return id;
}

void Record::setId(int id) {
    this->id = id;
}