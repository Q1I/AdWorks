/* 
 * File:   Auswertung_Korpus.cpp
 * Author: ran
 * 
 * Created on 26. Januar 2013, 17:46
 */

#include "Auswertung_Korpus.h"

Auswertung_Korpus::Auswertung_Korpus() {
}

Auswertung_Korpus::Auswertung_Korpus(const Auswertung_Korpus& orig) {
}

Auswertung_Korpus::~Auswertung_Korpus() {
}

// hab ich neu reingenommen das is der pfad fuer unser lda-input
const std::string Auswertung_Korpus::LDA_INPUT_FILE = "resources/lda-input/lda-input.txt";

void Auswertung_Korpus::readCorpusFiles(boost::filesystem::path &korpus) {
    // als erstes Remove old lda_input file, damit wir nicht text an alte lda-input-datei ranhaengen
    if (std::remove(this->LDA_INPUT_FILE.c_str()) != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");

    //Dateien aus Ordner holen
    if (!boost::filesystem::exists(korpus)) {
        std::cerr << "Dieser Ordner existiert nicht!" << std::endl;
    }

    std::vector<std::string> words;
    if (boost::filesystem::is_directory(korpus)) {
        boost::filesystem::directory_iterator end;
        for (boost::filesystem::directory_iterator iter(korpus); iter != end; ++iter) {
            std::cout << ">>Current Position: " << iter->path() << std::endl;

            std::ostringstream fileName;
            fileName << iter->path();
            std::ifstream reader;
            reader.open(fileName.str().c_str());
            //            reader.open("resources/korpus/was_war1.txt");
            std::string buffer;
            std::string all;
            if (reader.is_open()) {
                std::cout << "File opened" << std::endl;
                while (reader.good()) {
                    getline(reader, buffer); // buffer bekommt jedes mal neuen wert -> zeile wird nichtan buffer gehangen
                    // all is der string mit dem kompletten text der datei
                    all += buffer;
                    //                    std::cout << "Eingelesen: " << buffer << std::endl;
                }

                // split
                boost::algorithm::split(words, all, boost::is_any_of("[.,?!; \t\n]"), boost::algorithm::token_compress_on);
                /*
                // test: ausgabe split
                for (std::vector<std::string>::iterator it = words.begin(); it != words.end(); it++) {
                    std::cout << *it << std::endl;
                    }
                */
                reader.close();
            } else {
                std::cerr << "Unable to open " << fileName.str().c_str() << std::endl;
            }

            countWords(words);
            words.clear();
        }
    }
    std::cout<<">>>Done reading file"<<std::endl;
}

void Auswertung_Korpus::countWords(std::vector<std::string> words) {
    std::cout << "countWords: words size=" << words.size() << std::endl;

    bool start_with;
    std::vector<std::string> wordList;
    std::vector<int> countsList;
    std::string word;
    //const std::string firstLetter = ; 
    const char* expression = "[A-Z]+";

    const boost::regex ex(expression);
    for (int i = 0; i < words.size(); i++) {
        word = (std::string)words[i];
        // std::cout << firstLetter << std::endl;
        if (boost::starts_with(word, ex)) {
            wordList.push_back(word);
            //schauen, ob Word bereits gefunden->
            //Anzahl dieses Wortes erhöhen
            for (int j = 0; j < wordList.size(); j++) {
                if (word == wordList[i]) {
                    if (countsList.size() <= (i + 1))
                        countsList.push_back(0);
                    std::cout << countsList[i] << std::endl;
                    countsList[i] == (int) countsList[i] + 1;
                    std::cout << countsList[i] << std::endl;
                    break;
                }

                wordList.push_back(word);
                countsList.push_back(1);
            }
            //wenn noch nicht, neu eintragen, mit neuer ID (=Listenposition)
            //Anzahl vorkommen auf 1 setzen

        }

    }

    writeFile(this->LDA_INPUT_FILE, wordList, countsList);
}

void Auswertung_Korpus::writeFile(std::string filename, std::vector<std::string> words, std::vector<int>counts) {

    std::cout << "write" << std::endl;
    std::ofstream lda_input(filename.c_str(), std::ios_base::app);
    if (lda_input.is_open() != true) {
        std::cerr << "Unable to open " << filename << std::endl;
    }
    //zeichen schreiben
    lda_input << ">>>>>>>>>>>>>size: " << words.size() << " ";
    // lda_input.put(words.size());
    // lda_input.put(" ");
    for (int i = 0; i < counts.size(); i++) {
        lda_input << i << ":" << counts[i] << " ";
        /*   lda_input.put(i);
           lda_input.put(":");
           lda_input.put(counts[i]);
           lda_input.put(" ");
         */
    }
    lda_input << std::endl;
    // lda_input.put("\n");
    lda_input.close();
    std::cout << "write done" << std::endl;
}