#ifndef TESTCSVREADER_H
#define TESTCSVREADER_H

#include <string>
#include <vector>
#include <QObject>
#include <QtTest/QtTest>

//Test the QString class
class TestQString:public QObject
{
    Q_OBJECT
private slots:
    void toUpper();
};

//Example Test class from online
class MyFirstTest: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    { qDebug("called before everything else"); }
    void myFirstTest()
    { QVERIFY(1 == 1); }
    void mySecondTest()
    { QVERIFY(1 != 2); }
    void cleanupTestCase()
    { qDebug("called after myFirstTest and mySecondTest"); }
};

#endif // CSVREADER_H
