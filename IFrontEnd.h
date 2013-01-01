/* 
 * File:   IFrontEnd.h
 * Author: q1
 *
 * Created on December 29, 2012, 6:20 PM
 */

#ifndef IFRONTEND_H
#define	IFRONTEND_H

#include "IQueryResult.h"
#include "IUser.h"
#include "IBackEnd.cpp"

class IFrontEnd {
public:
    virtual IQueryResult* matchAd(std::string query, const IUser*
            user = NULL, bool* foundAd = NULL) = 0;
    // ermittelt die Landing Page des Ads adID und erhöht seine Klickanzahl
    virtual std::string getAdURL(uint32_t adID) = 0;
    // Verwende das Log in file um den Klick-Graphen zu bauen und
    // ähnliche Queries zu finden
    virtual bool analyzeClickGraph(const std::string& file) = 0;
    // Berechne Wahrscheinlichkeiten demographischer Merkmale von
    // Webseiten
    virtual bool analyzeDemographicFeatures(const std::string &userFile, const std::string& visitFile) = 0;
    // setze das zu verwendende Backend
    virtual void setBackend(IBackEnd* backend) = 0;
};


#endif	/* IFRONTEND_H */

