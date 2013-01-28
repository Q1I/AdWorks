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
#include "IUser.h"
#include "IFrontEnd.h"
#include "Auswertung_Korpus.h"
//"matchad" "-q Bahn" "-q zUG" "-q tablet" "-a na" "-g na"
//matchad -q Bahn -q zUG -q tablet -a na -g na
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
    BackEnd* b = new BackEnd();
    //    b.getAdURL(id);
    FrontEnd f;
    f.setBackend(b);
    f.getAdURL(id);
}

std::string getValue(std::string str) {
    std::cout << "GetValue: " << str << std::endl;
    if (boost::starts_with(str, "-")) {
        std::cout << "getValue: Invalid Cmd! Form has to be: matchad -q <query>. <query> contains starts with '-'" << std::endl;
        return NULL;
    }
    if (str == "") {
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

    // Process Arguments: -a || -g || -q
    for (int i = 1; i < argc; i++) {
        if (boost::starts_with(argv[i], "-a")) {
            age = getValue(argv[i + 1]);
        } else if (boost::starts_with(argv[i], "-g")) {
            gender = getValue(argv[i + 1]);
        } else if (boost::starts_with(argv[i], "-q")) {
            std::string q = getValue(argv[i + 1]);
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

    IUser* user = new IUser(g, a);
    BackEnd b;
    b.matchAdRewrites(queries, user, NULL);
}

void query(int argc, char **argv) {
    std::cout << "#Query: argc= " << argc << std::endl;
    if (argc > 3) {
        std::cout << "Too many arguments!" << std::endl;
        return;
    }
    std::cout << "query: " << argv[2] << std::endl;
    IUser* user = new IUser(GENDER_NA, AGE_NA);
    BackEnd* b = new BackEnd();
    FrontEnd f;
    f.setBackend(b);
    f.matchAd(argv[2], user, NULL);
}

void load_click_data(int argc, char **argv) {
    std::cout << "#load_click_data: argc= " << argc << std::endl;
    if (argc > 3) {
        std::cout << "Too many arguments!" << std::endl;
        return;
    }
    std::cout << "click file: " << argv[2] << std::endl;

    BackEnd* b = new BackEnd();
    FrontEnd f;
    f.setBackend(b);

    f.analyzeClickGraph(argv[2]);

}

void performLDA(int argc, char **argv) {
    std::cout << "#perform LDA: argc= " << argc << std::endl;
    if (argc > 3) {
        std::cout << "Too many arguments!" << std::endl;
        return;
    }
    std::cout << "folder: " << argv[2] << std::endl;

    BackEnd* b = new BackEnd();
    FrontEnd f;
    f.setBackend(b);

    //    f.performLDA(argv[2]);
    boost::filesystem::path korpus_files = "resources/korpus"; //argv[2];

    Auswertung_Korpus* auswertung = new Auswertung_Korpus();
    auswertung->readCorpusFiles(korpus_files);
    //    std::system("lda est 0.01 40 settings.txt tmpfile-lda.txt random foo/");

}

int main(int argc, char **argv) {
    std::cout << "===================" << std::endl;
    std::string reloadStr = "reload";
    std::string visitStr = "visit";
    std::string matchadStr = "matchad";
    std::string queryStr = "query";
    std::string clickDataStr = "load_click_data";
    std::string ldaStr = "performLDA";

    for (int i = 0; i < argc; i++) {
        std::cout << i << ". arg=" << argv[i] << "." << std::endl;
    }

    // test -----------------------------------
    performLDA(argc, argv);


    if (argc > 1) {
        std::cout << "Checking command .. " << std::endl;
        if (argv[1] == reloadStr) {
            reload(argc, argv);
        } else if (argv[1] == visitStr) {
            visit(argc, argv);
        } else if (argv[1] == matchadStr) {
            matchad(argc, argv);
        } else if (argv[1] == queryStr)
            query(argc, argv);
        else if (argv[1] == clickDataStr)
            load_click_data(argc, argv);
        else if (argv[1] == ldaStr)
            performLDA(argc, argv);
        else {
            std::cout << "Unknown CMD" << std::endl;
            std::cout << "List of CMDs:\n----------------------------"
                    "\nInit DB: \t\treload <ads_file.csv> <bid_phrase_file.csv>"
                    "\nLoad Click Data: \tload_click_data <clickgraph.csv>"
                    "\nSimrank MatchAd: \tquery <query>"
                    "\nMatchAd: \t\tmatchad -q <query> -q <query> ... -a <age> -g <gender>"
                    "\nGetAdUrl: \t\tvisit <id>"
                    "\nPerform LDA: \t\tperformLDA <folder>" << std::endl;
        }
    }
}