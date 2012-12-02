#include "File_Queries.h"

File_Queries::File_Queries(std::vector<std::string> f) {
    //    std::cout << "New File_Ads: " << f.at(0) << " " <<f.at(1)  << " " << f.at(2)  << " " << f.at(3) << " " << f.at(4)  << " " << f.at(5) << " " << f.at(6)  << " " << f.at(7) << std::endl;
    this->adId = f.at(0);
    this->phrases = this->remove(f.at(1));
    this->offer = f.at(2);

}

File_Queries::File_Queries(std::string adId, std::string phrases,std::string offer) {
    //    std::cout << "New File_Ads: " << f.at(0) << " " <<f.at(1)  << " " << f.at(2)  << " " << f.at(3) << " " << f.at(4)  << " " << f.at(5) << " " << f.at(6)  << " " << f.at(7) << std::endl;
    this->adId = adId;
    this->phrases = phrases;
    this->offer = offer;

}

std::string File_Queries::remove(std::string str) {

    char c = '"';
    str.erase(std::remove(str.begin(), str.end(), c), str.end());
    return str;

}

std::string File_Queries::getQuery() {
    std::string query;
    query = "Insert ignore into Queries values (" + this->adId + ", '" + this->phrases + "', " + this->offer + ")";
    return query;
}
