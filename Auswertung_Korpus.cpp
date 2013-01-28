/* 
 * File:   Auswertung_Korpus.cpp
 * Author: ran
 * 
 * Created on 26. Januar 2013, 17:46
 */
#include "Auswertung_Korpus.h"
#include "IUser.h"

Auswertung_Korpus::Auswertung_Korpus() {
}

Auswertung_Korpus::Auswertung_Korpus(const Auswertung_Korpus& orig) {
}

Auswertung_Korpus::~Auswertung_Korpus() {
}

const std::string Auswertung_Korpus::LDA_INPUT_FILE = "resources/lda-input/lda-input.txt";
const std::string Auswertung_Korpus::LDA_OUTPUT_FILE = "resources/lda-output/final.beta";

const std::string Auswertung_Korpus::stopWords[] = 
{"Der", "Die", "Das", 
"Ich", "Du", "Er", "Sie", "Es", "Wir", 
"Was", "Wer", "Wann", "Wo", "Wie", 
"Ein", "Eine", "Einer", "Einem",
"In","Aus","Von","Zu","Bei"
"Da","Dort",
"Und"};

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

        // addToWordSet
        for (boost::filesystem::directory_iterator iter(korpus); iter != end; ++iter) {
            std::cout << ">>Current Position: " << iter->path() << std::endl;
            std::ostringstream fileName;
            fileName << iter->path();
            std::ifstream reader;
            reader.open(fileName.str().c_str());
            //                        reader.open("resources/korpus/test.txt");
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

                reader.close();
            } else {
                std::cerr << "Unable to open " << fileName.str().c_str() << std::endl;
            }
            addToWordSet(words);
            //            countWords(words);


            words.clear();
            //                        break;
        }

        // create unique wordList
        createWordIndex();

        // count
        for (boost::filesystem::directory_iterator iter(korpus); iter != end; ++iter) {
            std::cout << ">>Current Position: " << iter->path() << std::endl;
            std::ostringstream fileName;
            fileName << iter->path();
            std::ifstream reader;
            reader.open(fileName.str().c_str());
            std::string buffer;
            std::string all;
            if (reader.is_open()) {
                std::cout << "File opened" << std::endl;
                while (reader.good()) {
                    getline(reader, buffer);
                    all += buffer;
                }

                // split
                boost::algorithm::split(words, all, boost::is_any_of("[.,?!; \t\n]"), boost::algorithm::token_compress_on);

                reader.close();
            } else {
                std::cerr << "Unable to open " << fileName.str().c_str() << std::endl;
            }
            countWords(words);
            words.clear();
            //                        break;
        }
    }

    std::cout << ">>>Done reading file" << std::endl;
    std::cout << ">>>All words: " << wordIndex.size() << std::endl;
}

void Auswertung_Korpus::createWordIndex() {
    std::set<std::string>::iterator it;
    for (it = wordSet.begin(); it != wordSet.end(); it++) {
        wordIndex.insert(wordIndex.end(), *it);
    }

    // sort ..extra
    std::sort(wordIndex.begin(), wordIndex.end());

    std::cout << "wordIndex size=" << wordIndex.size() << "\n================================\n\n" << std::endl;
}

void Auswertung_Korpus::addToWordSet(std::vector<std::string> words) {
    std::cout << "addToWordSet: words size=" << words.size() << std::endl;
    std::cout << "before: wordSet size=" << wordSet.size() << std::endl;

    for (int i = 0; i < words.size(); i++)
        if (checkWord(words.at(i))) // longer than 1 and capital 
            wordSet.insert(wordSet.end(), words.at(i));

    std::cout << "after: wordSet size=" << wordSet.size() << std::endl;
}

int Auswertung_Korpus::getIndex(std::string word) {
    for (int i = 0; i < wordIndex.size(); i++) {
        if (word == wordIndex.at(i))
            return i;
    }
    return -1;
}

void Auswertung_Korpus::countWords(std::vector<std::string> words) {
    std::cout << "countWords: words size=" << words.size() << std::endl;

    // unique words list for this document -> provides index
    std::vector <std::string> uniqueWords;
    std::vector <int> uniqueWordsIndex;

    // create uniqueWords with string
    std::set <std::string> tmp;
    for (int i = 0; i < words.size(); i++)
        if (checkWord(words.at(i))) // longer than 1 and capital 
            tmp.insert(tmp.end(), words.at(i));
    std::set<std::string>::iterator it;
    for (it = tmp.begin(); it != tmp.end(); it++) {
        uniqueWords.insert(uniqueWords.end(), *it);
    }

    // create unieuqWordsIndex with int (psoition in gloabal index)
    for (int i = 0; i < uniqueWords.size(); i++) {
        uniqueWordsIndex.insert(uniqueWordsIndex.end(), getIndex(uniqueWords.at(i)));
    }
    std::cout << "uWordsIndex size = " << uniqueWordsIndex.size() << std::endl;

    // occurence counter 
    std::vector <int> counter(uniqueWords.size());
    // count
    for (int i = 0; i < uniqueWords.size(); i++) {
        for (int j = 0; j < words.size(); j++) {
            if (uniqueWords.at(i) == words.at(j)) {
                counter.at(i)++;
            }
        }
    }


    //    for (std::vector<std::string>::iterator it = uniqueWords.begin(); it != uniqueWords.end(); it++) {
    //        std::cout << *it << std::endl;
    //    }

    //    for (int i = 0; i < uniqueWords.size(); i++) {
    //        if (counter.at(i) > 1)
    //            std::cout << uniqueWords.at(i) << " : " << counter.at(i) << std::endl;
    //    }

    std::cout << "words size=" << words.size() << std::endl;
    std::cout << "uniqueWords size=" << uniqueWords.size() << std::endl;
    std::cout << "uniqueWordsIndex size=" << uniqueWordsIndex.size() << std::endl;
    std::cout << "counter size=" << counter.size() << std::endl;

    writeFile(this->LDA_INPUT_FILE, uniqueWordsIndex, counter);
}

void Auswertung_Korpus::writeFile(std::string filename, std::vector<int> words, std::vector<int>counts) {

    std::cout << "write: words size = " << words.size() << "\tcounts size = " << counts.size() << std::endl;
    std::ofstream lda_input(filename.c_str(), std::ios_base::app);
    if (lda_input.is_open() != true) {
        std::cerr << "Unable to open " << filename << std::endl;
    }
    //zeichen schreiben
    lda_input << words.size() << " ";
    for (int i = 0; i < counts.size(); i++) {
        lda_input << words.at(i) << ":" << counts[i] << " ";
    }
    lda_input << std::endl;
    lda_input.close();

    for (int i = 0; i < words.size(); i++) {
        if (counts.at(i) > 1)
            std::cout << wordIndex.at(words.at(i)) << " : " << counts.at(i) << std::endl;
    }

    std::cout << "write done" << std::endl;
}

void Auswertung_Korpus::setBackend(BackEnd * backend) {
    std::cout << "AK: Set Backend." << std::endl;
    this->backEnd = (BackEnd*) backend;
}

void Auswertung_Korpus::processLDA() {

    // read final.beta
    std::ifstream reader;
    reader.open(this->LDA_OUTPUT_FILE.c_str());
    std::string buffer;
    std::vector<std::string> val;
    int cluster = 0;
    if (reader.is_open()) {
        std::cout << "File opened" << std::endl;

        // truncate table
        if (!this->backEnd->connect())
            return;
        this->backEnd->dbInsert("TRUNCATE TABLE `LDA`");

        while (reader.good()) {
            getline(reader, buffer);
            // split
            boost::algorithm::split(val, buffer, boost::is_any_of(" "), boost::algorithm::token_compress_on);
            if (val.at(0).size() < 1) {
                val.erase(val.begin());
            }
            std::cerr << "val size: " << val.size() << std::endl;

            if (val.size() > 0) {
                // get Top 10
                top10(val, cluster);
            }
            val.clear();
            cluster++;
        }

        reader.close();
    } else {
        std::cerr << "Unable to open " << this->LDA_OUTPUT_FILE << std::endl;
    }

}

struct less_than {

    inline bool operator() (const LDA_VAL& struct1, const LDA_VAL & struct2) {
        return (struct1.val > struct2.val);
    }
};

void Auswertung_Korpus::top10(std::vector<std::string> val, int cluster) {

    std::vector<LDA_VAL> lda;

    for (int i = 0; i < val.size(); i++) {
        LDA_VAL l;
        l.pos = i;
        l.val = atof(val.at(i).c_str());
        lda.insert(lda.end(), l);
    }

    // sort
    std::sort(lda.begin(), lda.end(), less_than());

    // top 10
    std::cout << "Cluster "<<cluster<<". Insert QUERY:\n" <<std::endl;
    std::string query = "";
    for (int i = 0; i < 10; i++) {
        query += "INSERT INTO LDA VALUES('";
        query += wordIndex.at(lda.at(i).pos);
        query += "',";
        query += boost::lexical_cast<std::string > (cluster);
        query += ");";
        std::cout << query << std::endl;
        this->backEnd->dbInsert(query);
        query="";
    }
    
}

void Auswertung_Korpus::dbUpdate() {

}

bool Auswertung_Korpus::checkWord(std::string word) {
    bool ok = false;
    // size and uppercase
    if ((word.size() > 1) && (std::isupper(word[0])))
        ok = true;
    // stopp word

    for (int i = 0; i < 25; i++) { // size of array
        if (stopWords[i] == word)
            return false;
    }

    return ok;
}