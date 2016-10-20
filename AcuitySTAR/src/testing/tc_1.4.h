#ifndef TC_1_4_H
#define TC_1_4_H

#include <string>
#include <vector>
#include <QObject>
#include <QDebug>
#include <QtTest/QtTest>

class QTTestXXX : public QObject  {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
};

#endif // TC_1_4_H
