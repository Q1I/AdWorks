#include "IUser.h"

IUser::IUser(Gender gender, Age age) {
    this->gender = gender;
    this->age = age;
    //    this->browsingHistory=bHistory;
};

Gender IUser::getGender() {
    return this->gender;
};

Age IUser::getAge() {
    return this->age;
};

std::list<std::string> IUser::getBrowsingHistory() {
    return this->browsingHistory;
};


