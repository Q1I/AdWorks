/* 
 * File:   IUser.h
 * Author: q1
 *
 * Created on December 1, 2012, 11:55 PM
 */

#ifndef IUSER_H
#define	IUSER_H

#include <fstream>
#include <mysql++.h>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator
#include <list>   

enum Gender {
    GENDER_NA, GENDER_MALE, GENDER_FEMALE
};

enum Age {
    AGE_NA, AGE_TEEN, AGE_YOUNG, AGE_OLD
};

class IUser {
private:
    Gender gender;
    Age age;
    std::list<std::string> browsingHistory;
public:
    IUser(Gender,Age);
    Gender getGender();
    Age getAge();
    std::list<std::string> getBrowsingHistory();
};

#endif	/* IUSER_H */

