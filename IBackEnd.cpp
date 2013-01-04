#include "IBackEnd.h"

IQueryResult* BackEnd::matchAdRewrites(std::list<std::string> rewriteList, const IUser* user, bool*) {
    std::cout << "######MatchAdRewrites" << std::endl;
    std::cout << "age: " << user->getAge() << "\tgender: " << user->getGender() << std::endl;
    std::string q;
    std::vector<File_Queries> matchingAds;
    IQueryResult* ad;

    // Connect to db
    if (!this->connect()) {
        return ad;
    }
    // Matching
    for (std::list<std::string>::const_iterator iterator = rewriteList.begin(), end = rewriteList.end(); iterator != end; ++iterator) {
        q = *iterator;
        // Match with bid phrase
        std::vector<File_Queries> match = matching(q, user->getGender(), user->getAge());
        // merge with complete list of matching ads
        matchingAds.insert(matchingAds.end(), match.begin(), match.end());
    }
    std::cout << ">>>>>>>>>>>>>>>> Matching Result: Found " << matchingAds.size() << " ads" << std::endl;

    // If no matches were found, return
    if (matchingAds.size() == 0) {
        conn.disconnect();
        std::cout << ">>>>>>>>>>>>>>>> No Ads found! " << std::endl;
        return ad;
    }
    // Ranking
    File_Queries fq_ad = ranking(matchingAds);

    // Get QueryResult
    ad = getAd(atoi(fq_ad.adId.c_str()));

    // Increment Impressions
    incImpressions(atoi(fq_ad.adId.c_str()));

    conn.disconnect();

    return ad;
};

/**
 * Takes a bidPhrase and returns matched Ads in DB
 * @param bidPhrase
 * @return Vector of matched File_Queries
 */
std::vector<File_Queries> BackEnd::matching(std::string bidPhrase, Gender gender, Age age) {
    std::cout << "##dbMatching: bidPhrase = " << bidPhrase << std::endl;
    std::vector<File_Queries> results;
    //        std::set<int> results;
    std::string id, phrase, offer;
    // DB Query
    std::string q = "Select * from Queries where `Bid Phrase`= '" + bidPhrase + "'";
    std::cout << "query: " << q << std::endl;
    mysqlpp::Query query = conn.query(q);
    if (mysqlpp::StoreQueryResult res = query.store()) {
        //            cout << "query: Rows=" << res.num_rows() << " Fields=" << res.num_fields() << endl;
        for (size_t i = 0; i < res.num_rows(); ++i) {
            // Add results to vector
            res[i]["AdID"].to_string(id);
            res[i]["Bid Phrase"].to_string(phrase);
            res[i]["Gebot"].to_string(offer);
            File_Queries f(id, phrase, offer);
            // Check user information
            if (matchWithUserInformation(f, gender, age))
                results.push_back(f);
            //                results.insert(res[i]["AdID"]);
            //                std::cout << "AdID = " << res[i]["AdID"] << std::endl;
        }
    } else {
        std::cerr << "Error: " << query.error() << std::endl;
    }

    std::cout << ">> Number of matched ads for bidPhrase = " << bidPhrase << " is " << results.size() << std::endl;
    return results;
}

/**
 * Check if ad matches user's information (age, gender).
 * @param ad
 * @param gender
 * @param age
 * @return true/false
 */
bool BackEnd::matchWithUserInformation(File_Queries ad, Gender gender, Age age) {
    std::cout << "##matchWithUserInformation: ad = " << ad.adId << " gender: " << gender << " age:" << age << std::endl;

    std::string id, g, a;
    // Parse age and gender
    g = parseGender(gender);
    a = parseAge(age);
    std::cout << "Parsed: age = " << a << " gender: " << g << std::endl;

    // DB Query
    std::string q, qAge, qGender;
    if (a == "na") {
        qAge = "";
    } else {
        qAge = " AND (`Age` in ('na','" + a + "') OR `Age` like '%" + a + "%' )";
    }
    if (g == "na") {
        qGender = "";
    } else {
        qGender = " AND `Gender` in ('na','" + g + "')";
    }
    // complete query
    q = "Select * from Ads where `AdID`= '" + ad.adId +
            "'" + qAge + qGender;
    std::cout << "query: " << q << std::endl;
    mysqlpp::Query query = conn.query(q);
    if (mysqlpp::StoreQueryResult res = query.store()) {
        //            cout << "query: Rows=" << res.num_rows() << " Fields=" << res.num_fields() << endl;
        if (res.num_rows() > 0) {
            std::cout << "UInfo success!" << std::endl;
            return true;
        } else {
            std::cout << "UInfo fail!" << std::endl;
            return false;
        }
    } else {
        std::cerr << "Error: " << query.error() << std::endl;
    }

    return false;
}

/**
 * Parse gender -> enum to string conversion.
 * @param g
 * @return parsed gender string
 */
std::string BackEnd::parseGender(int g) {
    std::string gender = "na";
    if (g == GENDER_NA)
        gender = "na";
    else if (g == GENDER_MALE)
        gender = "male";
    else if (g == GENDER_FEMALE)
        gender = "female";
    return gender;
}

/**
 * Parse age -> enum to string conversion.
 * @param a
 * @return parsed age string
 */
std::string BackEnd::parseAge(int a) {
    std::string age = "na";
    if (a == AGE_NA)
        age = "na";
    else if (a == AGE_OLD)
        age = "old";
    else if (a == AGE_YOUNG)
        age = "young";
    else if (a == AGE_TEEN)
        age = "teen";
    return age;
}

/**
 * Returns best ranked ad from a vector of ads.
 * @param matchingAds vector of File_Queries
 * @return 
 */
File_Queries BackEnd::ranking(std::vector<File_Queries> matchingAds) {
    std::cout << "##Ranking Ads: Count = " << matchingAds.size() << std::endl;
    // ranking of an ad, tmpRank needed for comparing ranks
    double rank = 0, tmpRank = 0;
    File_Queries top, tmp;

    // Iterate over matchingAds vector
    std::vector<File_Queries>::iterator iter;
    for (iter = matchingAds.begin(); iter != matchingAds.end(); ++iter) {
        tmp = *iter;
        std::cout << "adId: " << tmp.adId << std::endl;
        // Calculate rank. formular: rank = CTRate * offer
        tmpRank = getCTRate(atoi(tmp.adId.c_str())) * getOffer(atoi(tmp.adId.c_str()), tmp.phrases);
        //            std::cout<<"r: "<<rank<<"\tt: "<<tmpRank<<std::endl;
        if (tmpRank >= rank) { // if found an ad with better rank -> set new ad as top-ad
            top = tmp;
            rank = tmpRank;
        }
        tmpRank = 0;
    }
    std::cout << ">>Ranking Top: adId = " << top.adId << "\tphrase = " << top.phrases << "\toffer = " << top.offer << std::endl;
    return top;
}

/**
 * Get + calculate CTRate
 * @param adId
 * @return CTRate
 */
double BackEnd::getCTRate(int adId) {
    std::cout << "##CTRate: adId = " << adId << std::endl;
    int clicks, impr;
    double rate = 0;
    // DB Query
    std::string id = boost::lexical_cast<std::string > (adId);
    std::string q = "Select `Anzahl Klicks`,`Anzahl Impressions` from Ads where `AdID`= " + id;
    std::cout << "CTrate query: " << q << std::endl;
    mysqlpp::Query query = conn.query(q);
    if (mysqlpp::StoreQueryResult res = query.store()) {
        //            cout << "query: Rows=" << res.num_rows() << " Fields=" << res.num_fields() << endl;
        for (size_t i = 0; i < res.num_rows(); ++i) {
            clicks = res[i]["Anzahl Klicks"];
            impr = res[i]["Anzahl Impressions"];
        }
    } else {
        std::cerr << "Error: " << query.error() << std::endl;
    }
    // CTR calculation
    if (impr != 0)
        rate = (double) clicks / (double) impr;
    std::cout << ">>CTRate: Clicks = " << clicks << "\tImpr = " << impr << "\tCTRate = " << rate << std::endl;

    return rate;
}

/**
 * Get offer from an ad
 * @param adId
 * @param phrases
 * @return offer
 */
double BackEnd::getOffer(int adId, std::string phrases) {
    std::cout << "##GetOffer: adId = " << adId << std::endl;
    double offer = 0;
    std::string id = boost::lexical_cast<std::string > (adId);
    std::string q = "Select `Gebot` from Queries where `AdID`= " + id;
    std::cout << "Offer query: " << q << std::endl;
    mysqlpp::Query query = conn.query(q);
    if (mysqlpp::StoreQueryResult res = query.store()) {
        //            cout << "query: Rows=" << res.num_rows() << " Fields=" << res.num_fields() << endl;
        for (size_t i = 0; i < res.num_rows(); ++i) {
            offer = res[i]["Gebot"];
        }
    } else {
        std::cerr << "Error: " << query.error() << std::endl;
    }

    std::cout << ">>Got Offer = " << offer << std::endl;

    return offer;
}

/**
 * Returns ad as IQueryResult
 * @param adId
 * @return ad
 */
IQueryResult* BackEnd::getAd(int adId) {
    std::cout << "##GetAd: adId = " << adId << std::endl;
    std::string title, creative;
    std::string id = boost::lexical_cast<std::string > (adId);
    std::string q = "Select * from Ads where `AdID`= " + id;
    std::cout << "Ad query: " << q << std::endl;
    mysqlpp::Query query = conn.query(q);
    if (mysqlpp::StoreQueryResult res = query.store()) {
        //            cout << "query: Rows=" << res.num_rows() << " Fields=" << res.num_fields() << endl;
        // save values to title and creative
        res[0]["Titel"].to_string(title);
        res[0]["Slogan"].to_string(creative);
        IQueryResult *ad = new IQueryResult(title, creative, (uint32_t) adId);
        std::cout << ">>>>>>>>>>>>>>>>> Ad: >>>>>>>>>>>>>>>>>>>> \nadId = " << adId << "\ntitle = " << ad->getTitle() << "\ncreative = " << ad->getCreative() << std::endl;
        return ad;
    } else {
        std::cerr << "Error: " << query.error() << std::endl;
    }
    IQueryResult* a;
    return a;
}

/**
 * Increment impression of an ad
 * @param adId
 */
void BackEnd::incImpressions(int adId) {
    std::string id = boost::lexical_cast<std::string > (adId);
    std::cout << "##Increment Impressions: adId = " << id << std::endl;
    std::string q = "UPDATE Ads SET `Anzahl Impressions`=`Anzahl Impressions`+1 WHERE AdID=" + id;
    mysqlpp::Query query = conn.query(q);
    mysqlpp::SimpleResult res = query.execute();
    std::cout << "Info: " << res.info() << std::endl;
}

/**
 * Get ad and increment its click counter.
 * @param adID
 * @return url
 */
std::string BackEnd::getAdURL(uint32_t adID) {
    std::string id = boost::lexical_cast<std::string > (adID); // int to string cast
    std::string q = "Select URL from Ads where AdID=" + id;
    std::cout << "######Get AdURL: " << q << std::endl;

    // Get Connection to mysql
    std::cout << "Open Connection ..." << std::endl;
    try {
        if (conn.connect("Adworks", "localhost", "root", "root"))
            std::cout << "Connection success." << std::endl;
        else {
            std::cout << "Connection failed." << std::endl;
            return NULL;
        }
    } catch (mysqlpp::Exception e) {
        std::cerr << "problem: " << e.what() << std::endl;
        return NULL;
    }

    // Get URL
    std::string url;
    mysqlpp::Query query = conn.query(q);
    if (mysqlpp::StoreQueryResult res = query.store()) {
        //            cout << "query: Rows=" << res.num_rows() << " Fields=" << res.num_fields() << endl;
        for (size_t i = 0; i < res.num_rows(); ++i) {
            // save value into url
            res[i]["URL"].to_string(url);
            break;
        }
    } else {
        std::cerr << "Failed to get item list: " << query.error() << std::endl;
    }

    // Increase clicks
    q = "UPDATE Ads SET `Anzahl Klicks`=`Anzahl Klicks`+1 WHERE AdID=" + id;
    std::cout << "Query: " << q << std::endl;
    mysqlpp::Query query2 = conn.query(q);
    mysqlpp::SimpleResult res = query2.execute();
    std::cout << "Increment Clicks for AdID=" + id << std::endl;
    std::cout << "Status: " << res.info() << std::endl;

    conn.disconnect();

    std::cout << ">>>>>>>>>>>>>>>>> URL: >>>>>>>>>>>>>>>>>>>> \nadId = " << id << std::endl;
    std::cout << "URL = " << url << std::endl;

    return url;
};

bool BackEnd::initDatabase(const std::string& adFile, const std::string& bidPhraseFile) {
    using namespace boost;

    std::cout << "######InitDatabase ..." << std::endl;

    // Check if db exists
    if (!checkDB())
        if(!createDB())
            return false;
    
    // Get Connection to mysql
    if (!connect())
        return false;

    // AD file
    std::cout << "Ad File: " << adFile << std::endl;
    std::ifstream myfile;
    myfile.open(adFile.c_str());
    std::string line;
    std::vector <std::string> fields;
    if (myfile.is_open()) {
        std::cout << "File opened" << std::endl;
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
                std::cerr << "problem: " << e.what() << std::endl;
                return -1;
            }
        }
        myfile.close();
    } else
        std::cout << "Error: Couldn't open ad file" << std::endl;

    // Query file
    std::cout << "bidPhraseFile File: " << bidPhraseFile << std::endl;
    myfile.open(bidPhraseFile.c_str());
    if (myfile.is_open()) {
        std::cout << "File opened" << std::endl;
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
                std::cerr << "problem: " << e.what() << std::endl;
                return -1;
            }
        }
        myfile.close();
    } else
        std::cout << "Error: Couldn't open query file" << std::endl;
    conn.disconnect();
    return true;
};

/** 
 * Connect to database 
 * @return connecting-> success or fail
 */
bool BackEnd::connect() {
    std::cout << "Open Connection ..." << std::endl;
    try {
        if (conn.connect("Adworks", "localhost", "root", "root"))
            std::cout << "Connection success." << std::endl;
        else {
            std::cout << "Connection failed." << std::endl;
            return -1;
        }
    } catch (mysqlpp::Exception e) {
        std::cerr << "problem: " << e.what() << std::endl;
        return -1;
    }
    return true;
};

/** 
 * Connect to database 
 * @return connecting-> success or fail
 */
bool BackEnd::checkDB() {
    std::cout << "Check if db exists..." << std::endl;
    mysqlpp::Connection connection;
    try {
        if (connection.connect("", "localhost", "root", "root")) {
                std::cout << "Connected to mysql server: " << std::endl;
            std::string q = "SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = 'Adworks'";
            
            mysqlpp::Query query = connection.query(q);
            std::cout << "query" << std::endl;
            mysqlpp::StoreQueryResult result = query.store();
            if(result.size()>0){
                std::cout << "Db 'Adworks' does exist." << std::endl;
                return true;
            } else {
                std::cout << "Db doesn't exist." << std::endl;
                return false;
            }
        } else {
            std::cout << "Couldn't connect to mysql database!" << std::endl;

        }
    } catch (mysqlpp::Exception e) {
        std::cerr << "problem: " << e.what() << std::endl;
        return -1;
    }
    return false;
};

/** 
 * Create db 
 * @return creation-> success or fail
 */
bool BackEnd::createDB() {
    std::cout << "Create DB" << std::endl;
    try {
        mysqlpp::Connection connection;
        if (connection.connect("", "localhost", "root", "root"))
            std::cout << "Read sql file" << std::endl;
        std::string q = "";
        std::ifstream myfile;
        myfile.open("resources/schema.sql");
        std::string line;
        if (myfile.is_open()) {
            std::cout << "File opened" << std::endl;
            while (myfile.good()) {
                getline(myfile, line);
                q += line;
            }
            myfile.close();

            std::cout << "split q: "<<q << std::endl;
            std::vector<std::string> stmts;
            boost::split(stmts, q, boost::is_any_of(";"));
            std::cout<<"Stmts size: "<<stmts.size()<<std::endl;
            
            for(int i=0;i<stmts.size();i++){
            // Execute query
            mysqlpp::Query query = connection.query(stmts.at(i));
            
            mysqlpp::SimpleResult res = query.execute();
            std::cout << "Info: " << res.info() << std::endl;
            }
            return true;
        } else
            std::cout << "Error: Couldn't open SQL Schema file" << std::endl;
    } catch (mysqlpp::Exception e) {
        std::cerr << "problem: " << e.what() << std::endl;
        return -1;
    }
    return false;
};

/**
 * Execute insert query.
 * @param q insert statement
 */
void BackEnd::dbInsert(std::string q) {
    std::cout << "dbInsert: " << q << std::endl;
    mysqlpp::Query query = conn.query(q);
    mysqlpp::SimpleResult res = query.execute();
    std::cout << "Info: " << res.info() << std::endl;
};

 mysqlpp::Connection* BackEnd::getConnection(){
     return &conn;
 }

