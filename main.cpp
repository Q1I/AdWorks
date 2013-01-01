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
#include "IFrontEnd.cpp"

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

    // Process Arguments: -a || -g || -q
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

void load_click_data(int argc, char **argv){
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

int main(int argc, char **argv) {
    std::cout << "===================" << std::endl;
    std::string reloadStr = "reload";
    std::string visitStr = "visit";
    std::string matchadStr = "matchad";
    std::string queryStr = "query";
    std::string clickDataStr = "load_click_data";
    
    for(int i=0;i<argc;i++){
        std::cout<<i<<". arg="<<argv[i]<<"."<<std::endl;
    }
    
    if (argc > 1) {
        std::cout << "Checking command .. " << std::endl;
        if (argv[1] == reloadStr) {
            reload(argc, argv);
        } else if (argv[1] == visitStr) {
            visit(argc, argv);
        } else if (argv[1] == matchadStr){
            matchad(argc, argv);
        } else if (argv[1] == queryStr)
            query(argc, argv);
        else if (argv[1] == clickDataStr)
            load_click_data(argc, argv);
        else
            std::cout << "Unknown CMD" << std::endl;
    }
    
    load_click_data(argc, argv);
}




//#include <boost/numeric/ublas/blas.hpp>
//
//#include <boost/numeric/ublas/matrix.hpp>
//
//#include <boost/numeric/ublas/io.hpp>
//
//#include <boost/algorithm/string.hpp>
//
//#include <string>
//
//#include <iostream>
//
//#include <fstream>
//
//
//
//using namespace boost;
//
//int main(int argc, char* argv[])
//
//
// {
//
//    // Correct number of Args?
//
////    if (argc != 2)
//// {
////
////        std::cout << "Usage: load_click_data <click file>"<<argc << std::endl;
////
////        return 0;
////
////    }
//
//    // open file
//
//    std::ifstream inputfile("resources/clickgraph.csv");
//
//    if (!inputfile.is_open())
// {
//
//        std::cerr << "Error: Could not open file" << std::endl;
//
//    }
//
//    // create inpute file matix
//
//    numeric::ublas::matrix<std::string> filematrix(0, 3);
//
//    // input file string
//
//    std::string inputstring;
//
//    std::vector<std::string> strs;
//
//
//
//    // read input file into matrix
//
//    while (inputfile.good())
// {
//
//        getline(inputfile, inputstring);
//
//        split(strs, inputstring, is_any_of("\t "));
//
//        if (strs.size() == 3 && !iequals(strs[0], "#"))
// {
//
//            filematrix.resize(filematrix.size1() + 1, 3, true);
//
//            for (int a = 0; a < 3; a++)
// {
//
//                filematrix(filematrix.size1() - 1, a) = strs[a];
//
//            }
//
//        }
//
//    }
//
//    // close input file
//
//    inputfile.close();
//
//    // check if matrix contains data
//
//    if (filematrix.size1() == 0)
// {
//
//        std::cerr << "Error: No vaild data in file" << std::endl;
//
//        return 0;
//
//    }
//
//
//
//    // Define p matrix axis
//
//    numeric::ublas::vector<std::string> p_axis;
//
//    numeric::ublas::vector<bool> p_bid;
//
//    // fill p axis
//
//    for (unsigned int a = 0; a < filematrix.size1(); a++)
// {
//
//        bool found_bid_phrase = false, found_ad = false;
//
//        for (unsigned int b = 0; b < p_axis.size(); b++)
// {
//
//            // compare bid phrase
//
//            if (iequals(filematrix(a, 0), p_axis(b)))
// {
//
//                found_bid_phrase = true;
//
//            }
//
//            // compare ad
//
//            if (iequals(filematrix(a, 2), p_axis(b)))
// {
//
//                found_ad = true;
//
//            }
//
//            if (found_bid_phrase && found_ad) b = p_axis.size();
//
//        }
//
//        // add bid phrase
//
//        if (!found_bid_phrase)
// {
//
//            p_axis.resize(p_axis.size() + 1);
//
//            p_axis(p_axis.size() - 1) = filematrix(a, 0);
//
//            p_bid.resize(p_bid.size() + 1);
//
//            p_bid(p_bid.size() - 1) = true;
//
//        }
//
//        // add ad
//
//        if (!found_ad)
// {
//
//            p_axis.resize(p_axis.size() + 1);
//
//            p_axis(p_axis.size() - 1) = filematrix(a, 2);
//
//            p_bid.resize(p_bid.size() + 1);
//
//            p_bid(p_bid.size() - 1) = false;
//
//        }
//
//    }
//
//
//
//    // Deklare transition matrix P
//
//    numeric::ublas::matrix<float> p(p_axis.size(), p_axis.size());
//
//    for (unsigned int a = 0; a < p_axis.size(); a++)
//
//        for (unsigned int b = 0; b < p_axis.size(); b++)
//
//            p(a, b) = 0;
//
//    // Fill P
//
//    for (unsigned int a = 1; a < p_axis.size(); a++)
// {
//
//        for (unsigned int b = 0; b < a; b++)
// {
//
//            for (unsigned int c = 0; c < filematrix.size1(); c++)
// {
//
//                if ((iequals(filematrix(c, 0), p_axis(a)) && iequals(filematrix(c, 2), p_axis(b))) ||
//
//                        (iequals(filematrix(c, 0), p_axis(b)) && iequals(filematrix(c, 2), p_axis(a))))
// {
//
//                    p(a, b) = 1;
//
//                    p(b, a) = 1;
//
//                }
//
//            }
//
//        }
//
//    }
//
//
//
//    // normalize cols
//
//    for (unsigned int a = 0; a < p_axis.size(); a++)
// {
//
//        float c = 0;
//
//        for (unsigned int b = 0; b < p_axis.size(); b++)
// {
//
//            c += p(b, a);
//
//        }
//
//        for (unsigned int b = 0; b < p_axis.size(); b++)
// {
//
//            p(b, a) /= c;
//
//        }
//
//    }
//
//    // Create I as indentity matrix
//
//    numeric::ublas::identity_matrix<float> i(p_axis.size(), p_axis.size());
//
//
//
//    numeric::ublas::matrix<float> s = i;
//
//    // create temp matrix
//
//    numeric::ublas::matrix<float> temp(p_axis.size(), p_axis.size());
//
//    numeric::ublas::matrix<float> temp_diag(p_axis.size(), p_axis.size());
//
//    numeric::ublas::matrix<float> pt = trans(p);
//
//    // iterations, k = 5
//
//    for (unsigned int n = 0; n < 5; n++)
// {
//
//        temp = prod(pt, s);
//
//        temp = 0.8 * prod(temp, p);
//
//        //temp = (0.8 * prod(prod(pt,s),p));
//
//        // Diag(diag(temp))
//
//        temp_diag = temp;
//
//        for (unsigned int a = 0; a < p_axis.size(); a++)
// {
//
//            for (unsigned int b = 0; b < p_axis.size(); b++)
// {
//
//                if (a != b) temp_diag(a, b) = 0;
//
//            }
//
//        }
//
//        s = temp + i - temp_diag;
//
//    }
//
//    // show results
//
//    for (unsigned int a = 0; a < p_axis.size(); a++)
// {
//
//        if (p_bid(a))
// {
//
//            std::cout << p_axis(a) << ": " << std::endl;
//
//            for (unsigned c = 0; c < 5; c++)
// {
//
//                float max = 0;
//
//                unsigned int max_id = 0;
//
//                for (unsigned int b = 0; b < p_axis.size(); b++)
// {
//
//                    if (p_bid(b) && (a != b))
// {
//
//                        if (s(a, b) > max)
// {
//
//                            max = s(a, b);
//
//                            max_id = b;
//
//                        }
//
//                    }
//
//                }
//
//                std::cout << " " << p_axis(max_id) << ": " << max << std::endl;
//
//                s(a, max_id) = 0;
//
//            }
//
//        }
//
//    }
//
//    // Print matrix csv
//
//    std::cout << "Matrix:::::\n";
//
//    for (unsigned int a = 0; a < p_axis.size(); a++) std::cout << p_axis(a) << ","<<std::endl;
//
//    std::cout << std::endl;
//
//    std::cout << "axisss: \n\n";
//    for (unsigned int a = 0; a < p_axis.size(); a++)
// {
//
//        std::cout << p_axis(a) << ": ";
//
//        for (unsigned int b = 0; b < p_axis.size(); b++) std::cout << s(a, b) << ",";
//
//        std::cout << std::endl;
//
//    }
//
//    return 0;
//
//}