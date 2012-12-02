/* 
 * File:   File_Ads.h
 * Author: q1
 *
 * Created on December 2, 2012, 12:02 AM
 */

#ifndef FILE_ADS_H
#define	FILE_ADS_H

#include <mysql++.h>
#include <algorithm>    // copy

class File_Ads {
public:
    File_Ads(std::vector<std::string> );
//    File_Ads(std::string , std::string , std::string , std::string , std::string ,
//            std::string , std::string , std::string );
    std::string adId;
    std::string title;
    std::string slogan;
    std::string url;
    std::string impressions;
    std::string clicks;
    std::string gender;
    std::string age;   
    std::string getQuery();
    std::string remove(std::string);
};

#endif	/* FILE_ADS_H */

