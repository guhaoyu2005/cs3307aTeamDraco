#include <vector>
#include <string>
#include <fstream>
#include "tc_8_1.h"

using namespace std;

void QTTestCSVWriter::initTestCase() {
    vector<string> fooHeader;
    vector<vector<string>> fooData;

    for (int i=0;i<10;i++) {
        fooHeader.push_back("dummy");
    }
    fooData.push_back(fooHeader);
    CSVWriter writer;

    writer.writeToCSV("test.csv", fooHeader, fooData);

    ifstream in;
    in.open("test.csv");
    if (in) {
        QVERIFY(1);
    }
    else
        QVERIFY(0);
    in.close();
    remove("test.csv");
}
