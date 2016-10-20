#include "./test_1.3.h"

using namespace std;
void QTTestMainWindow::teachInvalidLoad()
{
    CSVReader reader("../Project Information/Sample Data/Presentations_sample.csv");
    vector<string> header = reader.getHeaders();
    vector< vector<string> > all_data = reader.getData();
    std::string searchstring = "Program";
    QVERIFY(std::find(header.begin(), header.end(), searchstring) == header.end());
}

void QTTestMainWindow::publicationInvalidLoad()
{
    CSVReader reader("../Project Information/Sample Data/Presentations_sample.csv");
    vector<string> header = reader.getHeaders();
    vector< vector<string> > all_data = reader.getData();
    std::string searchstring = "Publication Status";
    QVERIFY(std::find(header.begin(), header.end(), searchstring) == header.end());
}

void QTTestMainWindow::presentationInvalidLoad()
{
    CSVReader reader("../Project Information/Sample Data/Grants_expanded.csv");
    vector<string> header = reader.getHeaders();
    vector< vector<string> > all_data = reader.getData();
    std::string searchstring = "Activity Type";
    QVERIFY(std::find(header.begin(), header.end(), searchstring) == header.end());

}

void QTTestMainWindow::fundInvalidLoad()
{
    CSVReader reader("../Project Information/Sample Data/Presentations_sample.csv");
    vector<string> header = reader.getHeaders();
    vector< vector<string> > all_data = reader.getData();
    std::string searchstring = "Funding Type";
    QVERIFY(std::find(header.begin(), header.end(), searchstring) == header.end());
}
