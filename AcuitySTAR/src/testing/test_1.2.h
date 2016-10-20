#ifndef TEST_1_2_H
#define TEST_1_2_H

#include <string>
#include <vector>
#include <QObject>
#include <QDebug>
#include <QtTest/QtTest>

/**
 * @brief TC_1.2 - The QTTestTreeViewClick class
 * @class QTTestTreeViewClick
 * @author James Wake
 * @date 2016/10/19
 * @details
 * TC_1.1 - QT Test Class for Tree View Clicks
 */
class QTTestTreeViewClick: public QObject
{
    Q_OBJECT
private slots:
    /**
     * @brief initTestCase
     * @class QTTestTreeViewClick
     * @author James Wake
     * @date 2016/10/20
     * @details
     * Initialize the Test Case
     */
    void initTestCase();

    /**
     * @brief cleanupTestCase
     * @class QTTestTreeViewClick
     * @author James Wake
     * @date 2016/10/20
     * @details
     * Cleanup Tests
     */
    void cleanupTestCase();
};

#endif // TEST_1_2_H
