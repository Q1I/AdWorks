#include "File_Ads.h"



File_Ads::File_Ads(std::vector<std::string> f) {
    //    std::cout << "New File_Ads: " << f.at(0) << " " <<f.at(1)  << " " << f.at(2)  << " " << f.at(3) << " " << f.at(4)  << " " << f.at(5) << " " << f.at(6)  << " " << f.at(7) << std::endl;
    this->adId = f.at(0);
    this->title = this->remove(f.at(1));
    this->slogan = this->remove(f.at(2));
    this->url = f.at(3);
    this->impressions = f.at(4);
    this->clicks = f.at(5);
    this->gender = f.at(6);
    this->age = f.at(7);
};

std::string File_Ads::remove(std::string str) {

    char c = '"';
    str.erase(std::remove(str.begin(), str.end(), c), str.end());
    return str;

};

std::string File_Ads::getQuery() {
    std::string query;
    query = "Insert ignore into Ads values (" + this->adId + ", '" + this->title + "', '" + this->slogan + "','" + this->url + "'," +
            this->impressions + "," + this->clicks + ",'" + this->gender + "','" + this->age + "')";
    //    query = "inst ";
    return query;
};