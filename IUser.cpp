#include "IUser.h"

IUser::IUser(Gender gender, Age age) {
    this->gender = gender;
    this->age = age;
    //    this->browsingHistory=bHistory;
};

Gender IUser::getGender() const {
    return this->gender;
};

Age IUser::getAge() const{
    return this->age;
};

std::list<std::string> IUser::getBrowsingHistory() const{
    return this->browsingHistory;
};


