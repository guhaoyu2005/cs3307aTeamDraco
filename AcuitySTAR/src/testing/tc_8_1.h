#ifndef TC_8_1_H
#define TC_8_1_H

#include <string>
#include <vector>
#include <QObject>
#include <QDebug>
#include <QtTest/QtTest>

#include "database/CSVWriter.h"

class QTTestCSVWriter: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
};


#endif // TC_8_1_H
