#include "./test.h"
using namespace std;

void TestQString::toUpper(){
    QString str = "Hello";
    QVERIFY(str.toUpper() == "HELLO");
}
