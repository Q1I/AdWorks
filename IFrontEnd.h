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
#include "IBackEnd.h"
#include "Auswertung_Korpus.h"
#include <algorithm>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

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
    virtual void setBackend(BackEnd* backend) = 0;
};


struct entry {
    std::string query;
    int click;
    std::string ad;
};

struct SIM_RANK {
    std::string query;
    double simrank;
};

enum Mode {
    MODE_SIMRANK, MODE_SIMRANK_PP
};

class FrontEnd : public IFrontEnd {
private:
    BackEnd* backEnd;
    std::vector<std::string> queriesIndex;
    std::vector<std::string> adsIndex;
    std::vector<entry> clickGraph; // clickgraph
    boost::numeric::ublas::matrix<double> S;
    double totalClicks;
    double maxClicks;
    Mode mode; // simrank or simrank++
public:

    IQueryResult* matchAd(std::string query, const IUser*
            user=NULL, bool* foundAd=NULL);

    // ermittelt die Landing Page des Ads adID und erhöht seine Klickanzahl

    std::string getAdURL(uint32_t adID);

    // Verwende das Log in file um den Klick-Graphen zu bauen und
    // ähnliche Queries zu finden

    bool analyzeClickGraph(const std::string& file);

    // Berechne Wahrscheinlichkeiten demographischer Merkmale von
    // Webseiten

    bool analyzeDemographicFeatures(const std::string &userFile, const std::string& visitFile);

    // setze das zu verwendende Backend

    void setBackend(BackEnd* backend);

    int getIndexQuery(std::string query);

    int getIndexAd(std::string ad) ;

    void simrank();

    void getTop5(int pos);

    boost::numeric::ublas::matrix<double> transitionMatrixSimrank(int n);

    boost::numeric::ublas::matrix<double> transitionMatrixSimrankPP(int n);

    double weight(boost::numeric::ublas::matrix_row<boost::numeric::ublas::matrix<double> > row, int i);

    double spread(boost::numeric::ublas::matrix_column<boost::numeric::ublas::matrix<double> > col);
    
    std::vector<double> calcProbability(boost::numeric::ublas::matrix_column<boost::numeric::ublas::matrix<double> > col);

    int countSameNeighbours(boost::numeric::ublas::matrix<double> P, int i, int j);
    
    void performLDA(const std::string &file);
    
    IQueryResult* matchAdLDA(std::string query, const IUser*
            user=NULL, bool* foundAd=NULL);
};
#endif	/* IFRONTEND_H */

