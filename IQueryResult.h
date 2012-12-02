/* 
 * File:   IQueryResult.h
 * Author: q1
 *
 * Created on December 1, 2012, 11:46 PM
 */

#ifndef IQUERYRESULT_H
#define	IQUERYRESULT_H

class IQueryResult {
public:
    virtual std::string getTitle() const = 0;
    virtual std::string getCreative() const = 0;
    virtual uint32_t getAdID() const = 0;
};

#endif	/* IQUERYRESULT_H */

