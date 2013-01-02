/* 
 * File:   IQueryResult.h
 * Author: q1
 *
 * Created on December 1, 2012, 11:46 PM
 */
#include <fstream>
#include <mysql++.h>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator

#ifndef IQUERYRESULT_H
#define	IQUERYRESULT_H

class IQueryResult {
private:
    std::string title;
    std::string creative;
    uint32_t adId;
public:
    IQueryResult();
    IQueryResult(std::string, std::string, uint32_t);
    std::string getTitle();
    std::string getCreative();
    uint32_t getAdID();
};

#endif	/* IQUERYRESULT_H */

