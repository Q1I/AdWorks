#include <iostream>
#include <stdint.h>
#include <list>
#include <fstream>
#include <mysql++.h>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iterator>
#include "IBackEnd.cpp"
#include "IUser.h"

//"matchad" "-q Bahn" "-q zUG" "-q tablet" "-a na" "-g na"
//"reload" "resources/ads.csv" "resources/bid_phrases.csv"

void reload(int argc, char **argv) {
    std::cout << "#Reload: argc= " << argc << std::endl;
    if (argc > 4) {
        std::cout << "Too many arguments!" << std::endl;
        return;
    }
    std::cout << "ad: " << argv[2] << "\tphrase: " << argv[3] << std::endl;
    BackEnd b;
    b.initDatabase(argv[2], argv[3]);
}

void visit(int argc, char **argv) {
    std::cout << "#Visit: argc= " << argc << std::endl;
    if (argc > 3) {
        std::cout << "Too many arguments!" << std::endl;
        return;
    }
    std::cout << "id: " << argv[2] << std::endl;
    int id = atoi(argv[2]);
    BackEnd b;
    b.getAdURL(id);
}

std::string getValue(std::string str) {
    std::cout<<"GetValue: "<<str<<std::endl;
    if(boost::starts_with(str, "-")){
        std::cout << "getValue: Invalid Cmd! Form has to be: matchad -q <query>. <query> contains starts with '-'" << std::endl;
        return NULL;
    }
     if(str==""){
        std::cout << "getValue: Invalid Cmd! Form has to be: matchad -q <query>. <query> is empty" << std::endl;
        return NULL;
    }
        
    std::string value = str;
    std::cout << "value: " << value << std::endl;
    return value;
}

void matchad(int argc, char **argv) {
    std::cout << "#MatchAd: argc= " << argc << std::endl;
    std::list<std::string> queries;

    // -q
    if (!boost::starts_with(argv[2], "-q")) {
        std::cout << "Invalid Cmd! Form: matchad -q <query>" << std::endl;
        return;
    }

    // <query>
    //    std::string query = getValue(argv[2]);
    //    std::cout << "argv[2]: " << argv[2] << std::endl;
    //    std::cout << "query: " << query << std::endl;
    //    queries.insert(queries.end(),query);
    std::string age, gender;

    // -a || -g
    for (int i = 1; i < argc; i++) {
        if (boost::starts_with(argv[i], "-a")) {
            age = getValue(argv[i+1]);
        } else if (boost::starts_with(argv[i], "-g")) {
            gender = getValue(argv[i+1]);
        } else if (boost::starts_with(argv[i], "-q")) {
            std::string q = getValue(argv[i+1]);
            queries.insert(queries.end(), q);
        }
    }
    std::cout << "age: " << age << std::endl;
    std::cout << "gender: " << gender << std::endl;
    std::cout << "queries: " << queries.size() << std::endl;
    
    // Parse age and gender
    Age a;
    Gender g;
    if (age == "na")
        a = AGE_NA;
    else if (age == "old")
        a = AGE_OLD;
    else if (age == "young")
        a = AGE_YOUNG;
    else if (age == "teen")
        a = AGE_TEEN;
    if (gender == "male")
        g = GENDER_MALE;
    else if (gender == "female")
        g = GENDER_FEMALE;
    else if (gender == "na")
        g = GENDER_NA;

    IUser user(g, a);
    BackEnd b;
    b.matchAdRewrites(queries, user, NULL);
}

int main(int argc, char **argv) {
    std::cout << "===================" << std::endl;
    std::string reloadStr = "reload";
    std::string visitStr = "visit";
    std::string matchadStr = "matchad";
    
    for(int i=0;i<argc;i++){
        std::cout<<i<<". arg="<<argv[i]<<"."<<std::endl;
        
    }
    
    if (argc > 1) {
        std::cout << "Checking command .. " << std::endl;
        if (argv[1] == reloadStr) {
            reload(argc, argv);
        } else if (argv[1] == visitStr) {
            visit(argc, argv);
        } else if (argv[1] == matchadStr)
            matchad(argc, argv);
        else
            std::cout << "Unknown CMD" << std::endl;
    }
   
}


