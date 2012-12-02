/* 
 * File:   IUser.h
 * Author: q1
 *
 * Created on December 1, 2012, 11:55 PM
 */

#ifndef IUSER_H
#define	IUSER_H
enum Gender {
    GENDER_NA, GENDER_MALE, GENDER_FEMALE
};

enum Age {
    AGE_NA, AGE_TEEN, AGE_YOUNG, AGE_OLD
};

class IUser {
public:
    virtual Gender getGender() const = 0;
    virtual Age getAge() const = 0;
    virtual std::list<std::string> getBrowsingHistory() const = 0;
};



#endif	/* IUSER_H */

