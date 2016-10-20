#include "./test.h"
using namespace std;

void TestQString::toUpper(){
    QString str = "Hello";
    QVERIFY(str.toUpper() == "HELLO");
}

void QTTestCSVReader::initTestCase()
{
    qDebug("TC_1.1 Test CSV Starting");
}

