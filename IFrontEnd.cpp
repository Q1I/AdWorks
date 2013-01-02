#include "IFrontEnd.h"

IQueryResult* FrontEnd::matchAd(std::string query, const IUser*
        user, bool*) {
    std::list<std::string> queries;
    queries.insert(queries.end(), query);
    this->backEnd->matchAdRewrites(queries, user, NULL);
}

// ermittelt die Landing Page des Ads adID und erhöht seine Klickanzahl

std::string FrontEnd::getAdURL(uint32_t adID) {
    std::cout << "FE: getAdURL";
    return this->backEnd->getAdURL(adID);
}

// Verwende das Log in file um den Klick-Graphen zu bauen und
// ähnliche Queries zu finden

bool FrontEnd::analyzeClickGraph(const std::string& file) {
    std::cout << "FE: analyzeClickGraph" << std::endl;
    totalClicks = 0;
    maxClicks = 0;
    // Read file
    std::cout << "FE: Read file " << file << std::endl;
    std::ifstream myfile;
    myfile.open(file.c_str());
    //                myfile.open("resources/test.csv");
    //        myfile.open("resources/clickgraph.csv");
    std::string line;
    std::vector <std::string> fields;
    std::set<std::string> queries, ads;

    if (myfile.is_open()) {
        std::cout << "File opened" << std::endl;
        while (myfile.good()) {
            //                std::cout << "Get Line" << std::endl;
            getline(myfile, line);
            if (boost::starts_with(line, "#")) {
                std::cout << "Starts with #" << std::endl;
                continue;
            }
            // Split line delimiter='\t', save elements into fields
            //                std::cout << "split... " << line << std::endl;
            boost::split(fields, line, boost::is_any_of("\t"));
            //                std::cout << "field.size: " << fields.size() << std::endl;

            if (fields.size() != 3)
                continue;

            // click graph
            entry e;
            e.query = fields.at(0);
            e.click = atoi(fields.at(1).c_str());
            e.ad = fields.at(2);
            clickGraph.insert(clickGraph.end(), e);

            // insert into totalClicks for calculating probability 
            totalClicks += e.click;
            if (e.click > maxClicks) {
                maxClicks = e.click;
            }
            // insert into queries
            queries.insert(e.query);
            // insert into ads
            ads.insert(e.ad);
        }
        myfile.close();

        // Index: copy set into list
        std::cout << "ClickGraph: " << clickGraph.size() << std::endl;
        std::cout << "Queries: " << queries.size() << std::endl;
        std::cout << "Ads: " << ads.size() << std::endl;
        std::set<std::string>::iterator it;
        for (it = queries.begin(); it != queries.end(); it++) {
            queriesIndex.insert(queriesIndex.end(), *it);
            //                std::cout<<*it<<std::endl;
        }
        for (it = ads.begin(); it != ads.end(); it++) {
            adsIndex.insert(adsIndex.end(), *it);
        }
        std::cout << "QueriesIndex: " << queriesIndex.size() << std::endl;
        std::cout << "AdsIndex: " << adsIndex.size() << std::endl;

        simrank();

    } else
        std::cout << "Error: Couldn't open ad file" << std::endl;

    // reset
    queriesIndex.clear();
    adsIndex.clear();
    clickGraph.clear();
}

// Berechne Wahrscheinlichkeiten demographischer Merkmale von
// Webseiten

bool FrontEnd::analyzeDemographicFeatures(const std::string &userFile, const std::string& visitFile) {

}

// setze das zu verwendende Backend

void FrontEnd::setBackend(IBackEnd* backend) {
    std::cout << "FE: Set Backend." << std::endl;
    this->backEnd = backend;
}

int FrontEnd::getIndexQuery(std::string query) {
    for (int j = 0; j < queriesIndex.size(); j++) {
        //                    std::cout<<j<<". query: "<<queriesIndex.at(j)<<std::endl;
        if (queriesIndex.at(j) == query) {
            //                std::cout << "Found Query '" << query << "' at " << j << std::endl;
            return j;
        }
    }
    return -1;
}

int FrontEnd::getIndexAd(std::string ad) {
    for (int j = 0; j < adsIndex.size(); j++) {
        if (adsIndex.at(j) == ad) {
            //                std::cout << "Found Ad '" << ad << "' at " << j << std::endl;
            return j;
        }
    }
    return -1;
}

void FrontEnd::simrank() {
    std::cout << "#Simrank" << std::endl;
    using namespace boost::numeric::ublas;

    // N
    int n = queriesIndex.size() + adsIndex.size();

    // P matrix
    //        matrix<double> P = transitionMatrixSimrank(n); // simrank
    matrix<double> P = transitionMatrixSimrankPP(n); // simrank++

    // I Matrix
    identity_matrix<double> I(n);
    // S Matrix
    S = I;
    // P^T Matrix
    matrix<double> PT = trans(P);

    // tmp
    matrix<double> temp(n, n);
    matrix<double> diag(n, n);
    for (unsigned i = 0; i < n; ++i)
        for (unsigned j = 0; j < n; ++j)
            diag(i, j) = 0;

    // calc simrank
    std::cout << "Calculate Simrank" << std::endl;
    int k = 5;
    double c = 0.8;
    for (int i = 1; i < k; i++) {
        std::cout << i << ".temp: " << temp.size1() << "," << temp.size2() << "\tP: " << P.size1() << "," << P.size2() << "\tS: " << S.size1() << "," << S.size2() << std::endl;
        temp = prod(PT, S);
        temp = c * prod(temp, P);
        // diag
        for (int i = 0; i < n; i++) {
            diag(i, i) = temp(i, i);
        }
        //            std::cout << "diag:\n"<<diag << std::endl;
        S = temp + I - diag;
    }

    if (mode == MODE_SIMRANK_PP) { // if simrank++
        // evidence matrix V
        matrix<double> v = zero_matrix<int>(n, n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                double sum = 0;
                for (int k = 1; k <= countSameNeighbours(P, i, j); ++k)
                    sum += 1.0 / (pow(2, k));
                if (sum == 0) {
                    v(i, j) = 0.25;
                } else
                    v(i, j) = sum;
            }
        }
        // multiply with evidence matrix
        for (unsigned int i = 0; i < S.size1(); i++)
            for (unsigned int j = 0; j < S.size2(); j++) {
                if (i == j) {
                    S(i, j) = 1;
                    continue;
                }
                S(i, j) = S(i, j) * v(i, j);
            }
    }

    std::cout << "Simrank Matrix:\n" << S << std::endl;
    
    // Output
    std::string msg;
    if (mode == MODE_SIMRANK_PP) {
        msg = "Calculated with Simrank++";
    } else
        msg = "Calculated with Simrank";
    std::cout << "################## Top 5 Sim for a query #################" << std::endl;
    std::cout << msg << std::endl;
    for (int i = 0; i < queriesIndex.size(); i++) {
        std::cout << queriesIndex.at(i) << " : " << std::endl;
        getTop5(i);
    }
}

void FrontEnd::getTop5(int pos) {
    std::vector<SIM_RANK> rank;
    // get SIM_RANK
    for (int j = 0; j < queriesIndex.size(); j++) {
        SIM_RANK s;
        s.query = queriesIndex.at(j);
        s.simrank = S(pos, j);
        rank.insert(rank.end(), s);
    }

    // get Top 5
    while (rank.size() > 5) {
        // delete low simrank
        double min = 2;
        int posMin = 0;
        SIM_RANK tmp;
        for (int i = 0; i < rank.size(); i++) {
            //                std::cout<<i<<".tmp: query = "<<tmp.query<<" sim = "<<tmp.simrank<<std::endl;
            tmp = rank.at(i);
            if (tmp.simrank == 1) { // erase trivial ( simrank = 1)
                posMin = i;
                min = tmp.simrank;
                break;
            }
            if (tmp.simrank < min) {
                posMin = i;
                min = tmp.simrank;
            }
        }
        rank.erase(rank.begin() + posMin);
    }
    for (int i = 0; i < rank.size(); i++)
        std::cout << "\t" << rank.at(i).query << " = " << rank.at(i).simrank << std::endl;
}

boost::numeric::ublas::matrix<double> FrontEnd::transitionMatrixSimrank(int n) {
    std::cout << "Create Transition Matrix P" << std::endl;
    mode = MODE_SIMRANK; // simrank mode
    using namespace boost::numeric::ublas;
    matrix<double> P(n, n);
    // fill matrix with 0
    for (unsigned i = 0; i < P.size1(); ++i)
        for (unsigned j = 0; j < P.size2(); ++j)
            P(i, j) = 0;
    // iterate through click graph
    entry tmp;
    int posQuery, posAd;
    for (int i = 0; i < clickGraph.size(); i++) {
        tmp = clickGraph.at(i);
        //                std::cout<<">>>>>"<<i<<". entry: "<<tmp.ad <<"\t"<<tmp.query<<std::endl;

        // get index for matrix
        posQuery = getIndexQuery(tmp.query);
        if (posQuery == -1) {
            std::cout << "Error: Index Query = -1";
            continue;
        }
        posAd = getIndexAd(tmp.ad);
        if (posAd == -1) {
            std::cout << "Error: Index Ad = -1";
            continue;
        }
        // set transition in matrix p (1 means there is an edge between query and ad)
        P(posQuery, queriesIndex.size() + posAd) = 1;
        P(queriesIndex.size() + posAd, posQuery) = 1;
    }
    std::cout << "P (transition Matrix):\n" << P << std::endl;

    // normalize P
    for (int j = 0; j < n; j++) {
        float sum = 0;
        for (int i = 0; i < n; i++) {
            sum += P(i, j);
        }
        for (int i = 0; i < n; i++) {
            P(i, j) /= sum;
        }
    }
    std::cout << "Normalized P:\n" << P << std::endl;
    return P;
}

boost::numeric::ublas::matrix<double> FrontEnd::transitionMatrixSimrankPP(int n) {
    std::cout << "Simrank++: Create Transition Matrix P'" << std::endl;
    mode = MODE_SIMRANK_PP; // simrank++ mode
    using namespace boost::numeric::ublas;
    matrix<double> P(n, n);
    // fill matrix with 0
    for (unsigned i = 0; i < P.size1(); ++i)
        for (unsigned j = 0; j < P.size2(); ++j)
            P(i, j) = 0;
    // iterate through click graph
    entry tmp;
    int posQuery, posAd;
    for (int i = 0; i < clickGraph.size(); i++) {
        tmp = clickGraph.at(i);
        //                std::cout<<">>>>>"<<i<<". entry: "<<tmp.ad <<"\t"<<tmp.query<<std::endl;

        // get index for matrix
        posQuery = getIndexQuery(tmp.query);
        if (posQuery == -1) {
            std::cout << "Error: Index Query = -1";
            continue;
        }
        posAd = getIndexAd(tmp.ad);
        if (posAd == -1) {
            std::cout << "Error: Index Ad = -1";
            continue;
        }
        // set scaled weight
        P(posQuery, queriesIndex.size() + posAd) = tmp.click / maxClicks;
        P(queriesIndex.size() + posAd, posQuery) = tmp.click / maxClicks;
    }
    std::cout << "P' (scaled transition Matrix):\n" << P << std::endl;

    // copy P
    matrix<double> Pcopy = P;
    // calac weight P
    for (int a = 0; a < n; a++) {
        float sum = 0;
        for (int i = 0; i < n; i++) {
            if (P(a, i) == 0)
                continue;
            matrix_row<matrix<double> > row(Pcopy, a);
            matrix_column< matrix<double> > col(Pcopy, i);

            P(a, i) = spread(col) * weight(row, i);
            //                std::cout << "value: " << P(a, i) << "\n" << std::endl;
        }

    }
    std::cout << "weighted and spreaded P':\n" << P << std::endl;

    return P;
}

double FrontEnd::weight(boost::numeric::ublas::matrix_row<boost::numeric::ublas::matrix<double> > row, int i) {
    double sum = 0;
    for (int j = 0; j < row.size(); ++j) {
        sum += row(j);
    }
    return row(i) / sum;
}

double FrontEnd::spread(boost::numeric::ublas::matrix_column<boost::numeric::ublas::matrix<double> > col) {
    // Wahrscheinlichkeiten
    std::vector<double> probability = calcProbability(col);
    // Erwartungswert
    double expectation = 0;
    int c = 0;
    for (int i = 0; i < col.size(); i++) {
        if (col(i) != 0) {
            expectation += col(i) * probability.at(i);
            //                expectation += col(i);
            //                c++;
        }
    }
    //        expectation = expectation/c;

    // Varianz
    double variance = 0;
    int count = 0;
    double sum = 0;
    for (unsigned i = 0; i < col.size(); ++i) {
        if (col(i) != 0) {
            count++;
            variance += std::pow((col(i) - expectation), 2) * probability.at(i);
            //                variance += pow((col(i) - expectation), 2) ;
            //                std::cout << "variance: " << variance << " exp: " << expectation << " col: " << col(i) << " prob: " << probability.at(i) << " count: " << count << std::endl;
        }
    }
    //                variance = variance/count;

    // Spread
    double spread = exp(variance * (-1.0));
    //        std::cout << ">>>exp: " << expectation << std::endl;
    //        std::cout << "var: " << variance << std::endl;
    //        std::cout << "count: " << count << std::endl;
    //        std::cout << "spread: " << spread << std::endl;
    return spread;
}

std::vector<double> FrontEnd::calcProbability(boost::numeric::ublas::matrix_column<boost::numeric::ublas::matrix<double> > col) {
    std::vector<double> prob;
    double sum = 0;
    for (int i = 0; i < col.size(); i++) {
        sum += col(i);
    }
    for (int i = 0; i < col.size(); i++) {
        prob.insert(prob.end(), col(i) / sum);
    }
    return prob;
}

int FrontEnd::countSameNeighbours(boost::numeric::ublas::matrix<double> P, int i, int j) {
    int count = 0;
    for (unsigned k = 0; k < P.size1(); ++k)
        if (P(i, k) != 0 && P(j, k) != 0)
            ++count;
    return count;
}

