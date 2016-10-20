#ifndef TEST_1_4_H
#define TEST_1_4_H

#include <string>
#include <vector>
#include <QObject>
#include <QDebug>
#include <QtTest/QtTest>

class QTTestNewSortOrder : public QObject  {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void endTestCase();

    void createNewSortOrder();
};

#endif // TEST_1_4_H
