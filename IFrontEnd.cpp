#include "IFrontEnd.h"

IQueryResult* FrontEnd::matchAd(std::string query, const IUser*
        user, bool*) {
    // Rewrite query
    std::list<std::string> queries;

    if (!this->backEnd->connect())
        return NULL;
    std::cout << "conn: " << std::endl;

    // get similar queries from db
    std::string q = "SELECT Query,Query_1,Query_2,Query_3,Query_4,Query_5 FROM Simrank WHERE Query like '%" + query + "%'";
    std::cout << "Query: " << q << std::endl;
    mysqlpp::Query sqlQuery = this->backEnd->getConnection()->query(q);
    if (mysqlpp::StoreQueryResult res = sqlQuery.store()) {
        if (res.num_rows() > 0) {
            for (int i = 0; i < 6; i++) {
                queries.insert(queries.end(), res[0][i].c_str());
                std::cout << i << ".RES: " << res[0][i] << std::endl;
            }
        } else {
            queries.insert(queries.end(), query);
        }

        this->backEnd->matchAdRewrites(queries, user, NULL);
    }
}
// ermittelt die Landing Page des Ads adID und erhöht seine Klickanzahl

std::string FrontEnd::getAdURL(uint32_t adID) {
    std::cout << "FE: getAdURL";
    return this->backEnd->getAdURL(adID);
}

// Verwende das Log in file um den Klick-Graphen zu bauen und
// ähnliche Queries zu finden

bool FrontEnd::analyzeClickGraph(const std::string & file) {
    std::cout << "FE: analyzeClickGraph" << std::endl;
    totalClicks = 0;
    maxClicks = 0;

    // Read file
    std::cout << "FE: Read file " << file << std::endl;
    std::ifstream myfile;
    myfile.open(file.c_str());
    //                    myfile.open("resources/test.csv");
    //        myfile.open("resources/clickgraph.csv");
    std::string line;
    std::vector <std::string> fields;
    std::set<std::string> queries, ads; // using set for unique elements
    if (myfile.is_open()) {
        std::cout << "File opened" << std::endl;
        while (myfile.good()) {
            getline(myfile, line);
            if (boost::starts_with(line, "#")) {
                std::cout << "Starts with #" << std::endl;
                continue;
            }
            // Split line delimiter='\t', save elements into fields
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

            // insert into queries
            queries.insert(e.query);
            // insert into ads
            ads.insert(e.ad);

            // insert into totalClicks for calculating probability/ maxClick for scaling matrix
            totalClicks += e.click;
            if (e.click > maxClicks) {
                maxClicks = e.click;
            }
        }
        myfile.close();

        // Prepare Index: copy ads set and queries set into lists
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

        // simrank
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

bool FrontEnd::analyzeDemographicFeatures(const std::string &userFile, const std::string & visitFile) {

}

// setze das zu verwendende Backend

void FrontEnd::setBackend(BackEnd * backend) {
    std::cout << "FE: Set Backend." << std::endl;
    this->backEnd = (BackEnd*) backend;
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
    //            matrix<double> P = transitionMatrixSimrank(n); // simrank // here
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
    int k = 6;
    double c = 0.8;
    for (int i = 1; i < k; i++) {
        std::cout << i << ".temp: " << temp.size1() << "," << temp.size2() << "\tP: " << P.size1() << "," << P.size2() << "\tS: " << S.size1() << "," << S.size2() << std::endl;
        temp = prod(PT, S);
        temp = c * prod(temp, P);
        // diag
        for (int s = 0; s < n; s++) {
            diag(s, s) = temp(s, s);
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
                v(i, j) = sum;
            }
        }
        // multiply with evidence matrix
        for (int i = 0; i < S.size1(); i++)
            for (int j = 0; j < S.size2(); j++) {
                S(i, j) = S(i, j) * v(i, j);
            }
    }

    // Set diagonal of S to 1
    for (int i = 0; i < n; i++) {
        S(i, i) = 1;
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
    if (!this->backEnd->connect())
        return;
    // truncate simrank
    this->backEnd->dbInsert("TRUNCATE TABLE `Simrank`");
    for (int i = 0; i < queriesIndex.size(); i++) {
        std::cout << queriesIndex.at(i) << " : " << std::endl;
        getTop5(i);
    }
    std::cout << msg << std::endl;

}

// sort

struct less_than {

    inline bool operator() (const SIM_RANK& struct1, const SIM_RANK & struct2) {
        return (struct1.simrank > struct2.simrank);
    }
};

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

        // sort
        std::sort(rank.begin(), rank.end(), less_than());
    }
    for (int i = 0; i < rank.size(); i++)
        std::cout << "\t" << rank.at(i).query << " = " << rank.at(i).simrank << std::endl;

    // insert into db
    std::cout << "Insert top 5 into db" << std::endl;
    std::string q = "INSERT IGNORE INTO Simrank VALUES ('" + queriesIndex.at(pos) + "','" +
            rank.at(0).query + "'," + boost::lexical_cast<std::string > (rank.at(0).simrank) + ",'" +
            rank.at(1).query + "'," + boost::lexical_cast<std::string > (rank.at(1).simrank) + ",'" +
            rank.at(2).query + "'," + boost::lexical_cast<std::string > (rank.at(2).simrank) + ",'" +
            rank.at(3).query + "'," + boost::lexical_cast<std::string > (rank.at(3).simrank) + ",'" +
            rank.at(4).query + "'," + boost::lexical_cast<std::string > (rank.at(4).simrank) + ")";
    //    std::cout<<"Query: "<<q<<std::endl;
    this->backEnd->dbInsert(q);

    //    for (int i = 0; i < rank.size(); i++) {
    //        std::cout << "\t" << rank.at(i).query << " = " << rank.at(i).simrank << std::endl;
    //
    //    }
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
            //            if(i==j)
            //                P(i,j)=0;
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
        P(posQuery, queriesIndex.size() + posAd) = tmp.click;
        P(queriesIndex.size() + posAd, posQuery) = tmp.click;
    }
    std::cout << "P' (transition Matrix):\n" << P << std::endl;

    // Normalize
    for (int j = 0; j < n; j++) {
        float sum = 0;
        for (int i = 0; i < n; i++) {
            sum += P(i, j);
        }
        for (int i = 0; i < n; i++) {
            P(i, j) /= sum;
        }
    }

    // copy P
    matrix<double> Pcopy = P;
    // calc weight P
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
        }
    }

    // Varianz
    double variance = 0;
    int count = 0;
    double sum = 0;
    for (unsigned i = 0; i < col.size(); ++i) {
        if (col(i) != 0) {
            count++;
            variance += std::pow((col(i) - expectation), 2) * probability.at(i);
        }
    }

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

void FrontEnd::performLDA(const std::string & file) {
    Auswertung_Korpus* auswertung = new Auswertung_Korpus();
    auswertung->setBackend(this->backEnd);
    boost::filesystem::path path(file);
    auswertung->readCorpusFiles(path);
    std::system("./lda est 0.1 40 lda-settings.txt resources/lda-input/lda-input.txt seeded resources/lda-output/");
    auswertung->processLDA();
}

IQueryResult* FrontEnd::matchAdLDA(std::string query, const IUser*
        user, bool*) {
    // cluster list
    std::vector<std::string> clusters;

    // Rewrite query
    std::list<std::string> queries;

    std::set<std::string> unique;

    if (!this->backEnd->connect())
        return NULL;
    std::cout << "conn: " << std::endl;

    // get cluster from db
    std::string q = "SELECT Cluster FROM LDA WHERE Term like '%" + query + "%'";
    std::cout << "Cluster Query: " << q << std::endl;
    mysqlpp::Query sqlQuery = this->backEnd->getConnection()->query(q);
    if (mysqlpp::StoreQueryResult res = sqlQuery.store()) {
        if (res.num_rows() > 0) {
            for (int i = 0; i < res.num_rows(); i++) {
                clusters.insert(clusters.end(), res[i][0].c_str());
                std::cout << i << ".RES: " << res[i][0] << std::endl;
            }
        } else { // no cluster found
            queries.insert(queries.end(), query);
            this->backEnd->matchAdRewrites(queries, user, NULL);
            return NULL;
        }
    }

    // get terms
    std::string termQuery = "SELECT Term FROM LDA WHERE Cluster in (";
    for (int i = 0; i < clusters.size(); i++) {
        termQuery += (std::string) clusters.at(i);
        if (i != clusters.size() - 1)
            termQuery += " , ";
    }
    termQuery += ")";
    std::cout << "Term Query: " << termQuery << std::endl;
    mysqlpp::Query sqlTermQuery = this->backEnd->getConnection()->query(termQuery);
    if (mysqlpp::StoreQueryResult res = sqlTermQuery.store()) {
        if (res.num_rows() > 0) {
            for (int i = 0; i < res.num_rows(); i++) {
                unique.insert(res[i][0].c_str());
                std::cout << i << ".RES: " << res[i][0] << std::endl;
            }
        }
    }

    // copy unique into queries list
    std::copy(unique.begin(), unique.end(), std::back_inserter(queries));

    this->backEnd->matchAdRewrites(queries, user, NULL);
}