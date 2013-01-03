/* 
 * File:   IBackEnd.h
 * Author: q1
 *
 * Created on December 29, 2012, 6:02 PM
 */

#ifndef IBACKEND_H
#define	IBACKEND_H

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

class IBackEnd {
public:
    // siehe: IFrontEnd::matchAd
    virtual IQueryResult* matchAdRewrites(std::list<std::string> rewriteList, const IUser* user, bool* foundAd = NULL) = 0;
    // siehe: IFrontEnd::getAdURL
    virtual std::string getAdURL(uint32_t adID) = 0;
    // Datenbank mit Ads und Bid Phrases initialisieren
    virtual bool initDatabase(const std::string& adFile, const std::string& bidPhraseFile) = 0;
};

class BackEnd : public IBackEnd {
private:
    // Connection to mysql database
    mysqlpp::Connection conn;
public:
    IQueryResult* matchAdRewrites(std::list<std::string> rewriteList, const IUser* user, bool* foundAd = NULL);

    /**
     * Get ad and increment its click counter.
     * @param adID
     * @return url
     */
    std::string getAdURL(uint32_t adID);

    bool initDatabase(const std::string& adFile, const std::string& bidPhraseFile);
    
    /**
     * Takes a bidPhrase and returns matched Ads in DB
     * @param bidPhrase
     * @return Vector of matched File_Queries
     */
    std::vector<File_Queries> matching(std::string bidPhrase, Gender gender, Age age);

    /**
     * Check if ad matches user's information (age, gender).
     * @param ad
     * @param gender
     * @param age
     * @return true/false
     */
    bool matchWithUserInformation(File_Queries ad, Gender gender, Age age);
    
    /**
     * Parse gender -> enum to string conversion.
     * @param g
     * @return parsed gender string
     */
    std::string parseGender(int g);

    /**
     * Parse age -> enum to string conversion.
     * @param a
     * @return parsed age string
     */
    std::string parseAge(int a);

    /**
     * Returns best ranked ad from a vector of ads.
     * @param matchingAds vector of File_Queries
     * @return 
     */
    File_Queries ranking(std::vector<File_Queries> matchingAds);

    /**
     * Get + calculate CTRate
     * @param adId
     * @return CTRate
     */
    double getCTRate(int adId);

    /**
     * Get offer from an ad
     * @param adId
     * @param phrases
     * @return offer
     */
    double getOffer(int adId, std::string phrases);

    /**
     * Returns ad as IQueryResult
     * @param adId
     * @return ad
     */
    IQueryResult* getAd(int adId);

    /**
     * Increment impression of an ad
     * @param adId
     */
    void incImpressions(int adId);


    /** 
     * Connect to database 
     * @return connecting-> success or fail
     */
    bool connect();

    bool checkDB();
    
    bool createDB();
    /**
     * Execute insert query.
     * @param q insert statement
     */
    void dbInsert(std::string q);

     mysqlpp::Connection getConnection();
};

#endif	/* IBACKEND_H */

