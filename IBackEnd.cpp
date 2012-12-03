
#include <fstream>
#include <mysql++.h>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "IQueryResult.h"
#include "IUser.h"
#include "File_Ads.h"
#include "File_Queries.h"

using namespace std;

class IBackEnd {
public:
    // siehe: IFrontEnd::matchAd
    virtual IQueryResult matchAdRewrites(std::list<std::string> rewriteList, const IUser* user = NULL, bool* foundAd = NULL) = 0;
    // siehe: IFrontEnd::getAdURL
    virtual std::string getAdURL(uint32_t adID) = 0;
    // Datenbank mit Ads und Bid Phrases initialisieren
    virtual bool initDatabase(const std::string& adFile, const std::string& bidPhraseFile) = 0;
};

class BackEnd : public IBackEnd {
public:
    // Connection to mysql database
    mysqlpp::Connection conn;

    IQueryResult matchAdRewrites(std::list<std::string> rewriteList, const IUser* user = NULL, bool* foundAd = NULL) {
        std::cout << "######MatchAdRewrites" << std::endl;
        std::string q;
        std::vector<File_Queries> matchingAds;
        IQueryResult ad;
        
        // Connect to db
        if (!this->connect()){
            return ad;
        }
        // Matching
        for (std::list<std::string>::const_iterator iterator = rewriteList.begin(), end = rewriteList.end(); iterator != end; ++iterator) {
            q = *iterator;
            // Match with bid phrase
            std::vector<File_Queries> match = matching(q);
            matchingAds.insert(matchingAds.end(), match.begin(), match.end());
        }
        cout << ">>Found " << matchingAds.size() << " matched ads" << std::endl;

        // Ranking
        File_Queries fq_ad = ranking(matchingAds);

        // Get QueryResult
        ad = getAd(atoi(fq_ad.adId.c_str()));

        conn.disconnect();

        return ad;
    };

    File_Queries ranking(std::vector<File_Queries> matchingAds) {
        std::cout << "##Ranking Ads: Count = " << matchingAds.size() << std::endl;
        double rank = 0, tmpRank = 0;
        File_Queries top, tmp;

        std::vector<File_Queries>::iterator iter;
        for (iter = matchingAds.begin(); iter != matchingAds.end(); ++iter) {
            tmp = *iter;
            std::cout << "adId: " << tmp.adId << std::endl;
            // Calc rank
            tmpRank = getCTRate(atoi(tmp.adId.c_str())) * getOffer(atoi(tmp.adId.c_str()), tmp.phrases);
            //            std::cout<<"r: "<<rank<<"\tt: "<<tmpRank<<std::endl;
            if (tmpRank > rank) {
                top = tmp;
                rank = tmpRank;
            }
            rank = 0;
            tmpRank = 0;
        }
        std::cout << ">>Ranking Top: adId = " << top.adId << "\tphrase = " << top.phrases << "\toffer = " << top.offer << std::endl;
        return top;
    }

    std::vector<File_Queries> matching(std::string bidPhrase) {
        std::cout << "##dbMatching: bidPhrase = " << bidPhrase << std::endl;
        std::vector<File_Queries> results;
        //        std::set<int> results;
        std::string id, phrase, offer;
        int offerCompare = 0;
        std::string q = "Select * from Queries where `Bid Phrase`= '" + bidPhrase + "'";
        std::cout << "query: " << q << std::endl;
        mysqlpp::Query query = conn.query(q);
        if (mysqlpp::StoreQueryResult res = query.store()) {
            cout << "query: Rows=" << res.num_rows() << " Fields=" << res.num_fields() << endl;
            for (size_t i = 0; i < res.num_rows(); ++i) {
                res[i]["AdID"].to_string(id);
                res[i]["Bid Phrase"].to_string(phrase);
                res[i]["Gebot"].to_string(offer);
                File_Queries f(id, phrase, offer);
                results.push_back(f);
                //                results.insert(res[i]["AdID"]);
                //                std::cout << "AdID = " << res[i]["AdID"] << std::endl;
            }
        } else {
            cerr << "Error: " << query.error() << std::endl;
        }

        std::cout << ">>Number of matched ads = " << results.size() << std::endl;
        return results;
    }

    double getCTRate(int adId) {
        std::cout << "##CTRate: adId = " << adId << std::endl;
        int clicks, impr;
        double rate = 0;
        std::string id = boost::lexical_cast<std::string > (adId);
        std::string q = "Select `Anzahl Klicks`,`Anzahl Impressions` from Ads where `AdID`= " + id;
        std::cout << "CTrate query: " << q << std::endl;
        mysqlpp::Query query = conn.query(q);
        if (mysqlpp::StoreQueryResult res = query.store()) {
            cout << "query: Rows=" << res.num_rows() << " Fields=" << res.num_fields() << endl;
            for (size_t i = 0; i < res.num_rows(); ++i) {
                clicks = res[i]["Anzahl Klicks"];
                impr = res[i]["Anzahl Impressions"];
            }
        } else {
            cerr << "Error: " << query.error() << std::endl;
        }
        // CTR calculation
        if (impr != 0)
            rate = (double) clicks / (double) impr;
        std::cout << ">>CTRate: Clicks = " << clicks << "\tImpr = " << impr << "\tCTRate = " << rate << std::endl;

        return rate;
    }

    double getOffer(int adId, std::string phrases) {
        std::cout << "##GetOffer: adId = " << adId << std::endl;
        int offer = 0;
        std::string id = boost::lexical_cast<std::string > (adId);
        std::string q = "Select `Gebot` from Queries where `AdID`= " + id;
        std::cout << "Offer query: " << q << std::endl;
        mysqlpp::Query query = conn.query(q);
        if (mysqlpp::StoreQueryResult res = query.store()) {
            cout << "query: Rows=" << res.num_rows() << " Fields=" << res.num_fields() << endl;
            for (size_t i = 0; i < res.num_rows(); ++i) {
                offer = res[i]["Gebot"];
            }
        } else {
            cerr << "Error: " << query.error() << std::endl;
        }

        std::cout << ">>Got Offer = " << offer << std::endl;

        return (double) offer;
    }

    IQueryResult getAd(int adId) {
        std::cout << "##GetAd: adId = " << adId << std::endl;
        std::string title, creative;
        std::string id = boost::lexical_cast<std::string > (adId);
        std::string q = "Select * from Ads where `AdID`= " + id;
        std::cout << "Ad query: " << q << std::endl;
        mysqlpp::Query query = conn.query(q);
        if (mysqlpp::StoreQueryResult res = query.store()) {
            cout << "query: Rows=" << res.num_rows() << " Fields=" << res.num_fields() << endl;
            res[0]["Titel"].to_string(title);
            res[0]["Slogan"].to_string(creative);
            IQueryResult ad(title, creative, (uint32_t) adId);
            std::cout << ">> Ad: \nadId = " << adId << "\ntitle = "<<ad.getTitle()<<"\ncreative = "<<ad.getCreative()<<std::endl;
            return ad;
        } else {
            cerr << "Error: " << query.error() << std::endl;
        }
        IQueryResult a;
        return a;
    }

    std::string getAdURL(uint32_t adID) {
        std::string id = boost::lexical_cast<std::string > (adID);
        std::string q = "Select URL from Ads where AdID=" + id;
        std::cout << "######Get AdURL: " << q << std::endl;

        // Get Connection to mysql
        std::cout << "Open Connection ..." << std::endl;
        try {
            if (conn.connect("CA", "localhost", "root", "root"))
                std::cout << "Connection success." << std::endl;
            else {
                std::cout << "Connection failed." << std::endl;
                return NULL;
            }
        } catch (mysqlpp::Exception e) {
            std::cerr << "problem: " << e.what() << endl;
            return NULL;
        }

        // Get URL
        std::string url;
        mysqlpp::Query query = conn.query(q);
        if (mysqlpp::StoreQueryResult res = query.store()) {
            cout << "query: Rows=" << res.num_rows() << " Fields=" << res.num_fields() << endl;
            for (size_t i = 0; i < res.num_rows(); ++i) {
                res[i]["URL"].to_string(url);
                break;
            }
        } else {
            cerr << "Failed to get item list: " << query.error() << endl;
        }

        // Increase clicks
        q = "UPDATE Ads SET `Anzahl Klicks`=`Anzahl Klicks`+1 WHERE AdID=" + id;
        cout << "Query: " << q << std::endl;
        mysqlpp::Query query2 = conn.query(q);
        mysqlpp::SimpleResult res = query2.execute();
        std::cout << "Increment Clicks for AdID=" + id << std::endl;

        conn.disconnect();

        return url;
    };

    bool initDatabase(const std::string& adFile, const std::string& bidPhraseFile) {
        using namespace boost;

        std::cout << "######InitDatabase ..." << std::endl;
        // Get Connection to mysql
        if (!connect())
            return false;
        
        // AD file
        std::cout << "Ad File: " << adFile << std::endl;
        ifstream myfile;
        myfile.open(adFile.c_str());
        string line;
        vector <string> fields;
        if (myfile.is_open()) {
            cout << "File opened" << endl;
            while (myfile.good()) {
                std::cout << "Get Line" << std::endl;
                getline(myfile, line);
                // Split line delimiter='\t', save elements into fields
                std::cout << "split... " << std::endl;
                boost::split(fields, line, is_any_of("\t"));
                std::cout << "field.size: " << fields.size() << std::endl;
                if (fields.size() != 8)
                    continue;
                File_Ads ads(fields);
                // Insert into db
                try {
                    this->dbInsert(ads.getQuery());
                } catch (mysqlpp::Exception e) {
                    cerr << "problem: " << e.what() << endl;
                    return -1;
                }
            }
            myfile.close();
        } else
            cout << "Error: Open file" << endl;

        // Query file
        std::cout << "bidPhraseFile File: " << bidPhraseFile << std::endl;
        myfile.open(bidPhraseFile.c_str());
        if (myfile.is_open()) {
            cout << "File opened" << endl;
            while (myfile.good()) {
                std::cout << "Get Line" << std::endl;
                getline(myfile, line);
                // Split line delimiter='\t', save elements into fields
                std::cout << "split... " << std::endl;
                boost::split(fields, line, is_any_of("\t"));
                std::cout << "field.size: " << fields.size() << std::endl;
                if (fields.size() != 3)
                    continue;
                File_Queries q(fields);
                // Insert into db
                try {
                    this->dbInsert(q.getQuery());
                } catch (mysqlpp::Exception e) {
                    cerr << "problem: " << e.what() << endl;
                    return -1;
                }
            }
            myfile.close();
        } else
            cout << "Error: Query Open file" << endl;
        conn.disconnect();
        return true;
    };

    /** 
     * Connect to database 
     * @return connecting-> success or fail
     */
    bool connect() {
        std::cout << "Open Connection ..." << std::endl;
        try {
            if (conn.connect("CA", "localhost", "root", "root"))
                std::cout << "Connection success." << std::endl;
            else {
                std::cout << "Connection failed." << std::endl;
                return -1;
            }
        } catch (mysqlpp::Exception e) {
            cerr << "problem: " << e.what() << endl;
            return -1;
        }
        return true;
    };
    
    /**
     * Execute insert query.
     * @param q insert statement
     */
    void dbInsert(std::string q) {
        std::cout << "dbInsert: " << q << std::endl;
        mysqlpp::Query query = conn.query(q);
        mysqlpp::SimpleResult res = query.execute();
        cout << "Info: " << res.info() << std::endl;
    };

};