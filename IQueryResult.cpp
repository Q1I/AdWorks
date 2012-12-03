#include "IQueryResult.h"

IQueryResult::IQueryResult(std::string title, std::string creative, uint32_t adId) {
    this->title = title;
    this->creative = creative;
    this->adId = adId;
}

IQueryResult::IQueryResult() {
}

std::string IQueryResult::getTitle() {
    return this->title;
};

std::string IQueryResult::getCreative() {
    return this->creative;
};

uint32_t IQueryResult::getAdID() {
    return this->adId;
};



//
//QueryResult::QueryResult(std::string title, std::string creative, uint32_t adId) {
//    this->title = title;
//    this->creative = creative;
//    this->adId = adId;
//}
//
//QueryResult::QueryResult() {
//}
//
//std::string QueryResult::getTitle() {
//    return this->title;
//};
//
//std::string QueryResult::getCreative() {
//    return this->creative;
//};
//
//uint32_t QueryResult::getAdID() {
//    return this->adId;
//};
