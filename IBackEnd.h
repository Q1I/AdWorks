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



#endif	/* IBACKEND_H */

