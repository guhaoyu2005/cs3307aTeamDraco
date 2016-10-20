#include "./test_1.1.h"
using namespace std;

void QTTestCSVReader::initTestCase()
{
   qDebug("TC_1.1 Test CSV Starting");
}

void QTTestCSVReader::readGrants()
{
    CSVReader reader("../Project Information/Sample Data/Grants_expanded.csv");
    vector<string> header = reader.getHeaders();
    QVERIFY(header.size() != 0);
}

void QTTestCSVReader::readGrantsClinical()
{
    CSVReader reader("../Project Information/Sample Data/GrantsClinicalFunding_sample.csv");
    vector<string> header = reader.getHeaders();
    QVERIFY(header.size() != 0);
}

void QTTestCSVReader::readPresentations1()
{
    CSVReader reader("../Project Information/Sample Data/Presentations_expanded.csv");
    vector<string> header = reader.getHeaders();
    QVERIFY(header.size() != 0);
}

void QTTestCSVReader::readPresentations2()
{
    CSVReader reader("../Project Information/Sample Data/Presentations_sample.csv");
    vector<string> header = reader.getHeaders();
    QVERIFY(header.size() != 0);
}

void QTTestCSVReader::readTeaching()
{
    CSVReader reader("../Project Information/Sample Data/Program_Teaching_expanded.csv");
    vector<string> header = reader.getHeaders();
    QVERIFY(header.size() != 0);
}

void QTTestCSVReader::readNoFile(){
    CSVReader reader("NO FILE");
    vector<string> header = reader.getHeaders();
    QVERIFY(header.size() == 0);
}

void QTTestCSVReader::dataPresentations(){
    CSVReader reader("../Project Information/Sample Data/Presentations_sample.csv");
    vector< vector<string> > all_data = reader.getData();
    QVERIFY(all_data.size() != 0);
}

void QTTestCSVReader::dataTeaching(){
    CSVReader reader("../Project Information/Sample Data/Program_Teaching_expanded.csv");
    vector< vector<string> > all_data = reader.getData();
    QVERIFY(all_data.size() != 0);
 }

 void QTTestCSVReader::cleanupTestCase()
 {
    qDebug("TC_1.1 Test CSV Finished");
 }
