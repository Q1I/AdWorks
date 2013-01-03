CREATE DATABASE Adworks;

CREATE TABLE Adworks.Ads (
AdID INT NOT NULL ,
Titel VARCHAR( 200 ) NOT NULL ,
Slogan VARCHAR( 200 ) NOT NULL ,
URL VARCHAR( 100 ) NOT NULL ,
`Anzahl Impressions` INT NOT NULL ,
`Anzahl Klicks` INT NOT NULL ,
Gender INT NOT NULL ,
Age INT NOT NULL,
Primary Key (AdID)
);

CREATE TABLE Adworks.Queries (
AdID INT NOT NULL ,
`Bid Phrase` VARCHAR( 100 ) NOT NULL ,
Gebot DOUBLE NOT NULL,
Primary Key (AdID,`Bid Phrase`)
);

CREATE TABLE Adworks.Simrank (
Query VARCHAR( 100 ) NOT NULL ,
Query_1 VARCHAR( 100 ) NOT NULL ,
Score_1 DOUBLE NOT NULL,
Query_2 VARCHAR( 100 ) NOT NULL ,
Score_2 DOUBLE NOT NULL,
Query_3 VARCHAR( 100 ) NOT NULL ,
Score_3 DOUBLE NOT NULL,
Query_4 VARCHAR( 100 ) NOT NULL ,
Score_4 DOUBLE NOT NULL,
Query_5 VARCHAR( 100 ) NOT NULL ,
Score_5 DOUBLE NOT NULL,
Primary Key (Query)
);