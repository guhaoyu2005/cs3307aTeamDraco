#ifndef TESTCSVREADER_H
#define TESTCSVREADER_H

#include <string>
#include <vector>
#include <QObject>
#include <QDebug>
#include <QtTest/QtTest>

//Include for CSV Reader Testing
#include "./database/CSVReader.h"

//Include Tests
#include "test_1.1.h"
#include "test_1.2.h"
#include "test_1.3.h"
#include "test_1.4.h"

/**
 * @brief The TestQString class
 * An Example of Testing the QT String Class
 */
class TestQString:public QObject
{
    Q_OBJECT
private slots:
    void toUpper();
};


/**
 * @brief The MyFirstTest class
 * Example QT Test Case for reference (Comment this out)
 */
//class MyFirstTest: public QObject
//{
//    Q_OBJECT
//private slots:
//    void initTestCase()
//    { qDebug("called before everything else"); }
//    void myFirstTest()
//    { QVERIFY(1 == 1); }
//    void mySecondTest()
//    { QVERIFY(1 != 2); }
//    void cleanupTestCase()
//    { qDebug("called after myFirstTest and mySecondTest"); }
//};

#endif // CSVREADER_H
