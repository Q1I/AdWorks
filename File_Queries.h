/* 
 * File:   File_Queries.h
 * Author: q1
 *
 * Created on December 2, 2012, 12:05 AM
 */

#ifndef FILE_QUERIES_H
#define	FILE_QUERIES_H

#include <mysql++.h>
#include <algorithm>    // copy

class File_Queries {
public:
    File_Queries(std::vector<std::string> fields);
    std::string adId;
    std::string phrases;
    std::string offer;
    std::string getQuery();
    std::string remove(std::string);
};

#endif	/* FILE_QUERIES_H */

