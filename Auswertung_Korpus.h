/* 
 * File:   Auswertung_Korpus.h
 * Author: ran
 *
 * Created on 26. Januar 2013, 17:46
 */

#ifndef AUSWERTUNG_KORPUS_H
#define	AUSWERTUNG_KORPUS_H

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <set>
#include <ctype.h>
#include "IBackEnd.h"

struct LDA_VAL {
    int pos;
    double val;
};

class Auswertung_Korpus {
public:
    Auswertung_Korpus();
    Auswertung_Korpus(const Auswertung_Korpus& orig);
    virtual ~Auswertung_Korpus();

    static const std::string LDA_INPUT_FILE;
    static const std::string LDA_OUTPUT_FILE;
    
    static const std::string stopWords[]; 
    void readCorpusFiles(boost::filesystem::path &korpus);
    void readCorpusFiles(std::string korpuspfad);
    void writeFile(std::string filename, std::vector<int> words, std::vector<int> counts);
    void countWords(std::vector<std::string> words);
    void addToWordSet(std::vector<std::string> words);
    void createWordIndex();
    int getIndex(std::string word);
    void processLDA();
    void dbUpdate();
    void setBackend(BackEnd* backend);
    void top10(std::vector<std::string> val,int cluster);
    bool checkWord(std::string word);
private:
    std::set<std::string> wordSet;
    std::vector<std::string> wordIndex;
    BackEnd* backEnd;
};

#endif	/* AUSWERTUNG_KORPUS_H */

