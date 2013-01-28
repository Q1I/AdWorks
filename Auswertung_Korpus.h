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

class Auswertung_Korpus {
public:
    Auswertung_Korpus();
    Auswertung_Korpus(const Auswertung_Korpus& orig);
    virtual ~Auswertung_Korpus();

    static const std::string LDA_INPUT_FILE;
    void readCorpusFiles(boost::filesystem::path &korpus);
    void readCorpusFiles(std::string korpuspfad);
    void writeFile(std::string filename, std::vector<std::string> words, std::vector<int> counts);
    void countWords(std::vector<std::string> words);
private:

};

#endif	/* AUSWERTUNG_KORPUS_H */

