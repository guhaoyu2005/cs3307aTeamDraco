#ifndef TEST_1_4_H
#define TEST_1_4_H

#include <string>
#include <vector>
#include <QObject>
#include <QDebug>
#include <QtTest/QtTest>


/**
 * @brief The QTTestNewSortOrder class
 * @class QTTestNewSortOrder
 * @author Haoyu Gu
 * @date 2016/10/20
 * @details
 * QT Test Class for creating sort order function in mainwindow class
 */
class QTTestNewSortOrder : public QObject  {
    Q_OBJECT
private slots:
    /**
     * @brief initTestCase
     * @class QTTestNewSortOrder
     * @author Haoyu Gu
     * @date 2016/10/20
     * @details
     * Initialize the Test Case
     */
    void initTestCase();
    /**
     * @brief cleanupTestCase
     * @class QTTestNewSortOrder
     * @author Haoyu Gu
     * @date 2016/10/20
     * @details
     * Cleanup Tests
     */
    void cleanupTestCase();
    /**
     * @brief endTestCase
     * @class QTTestNewSortOrder
     * @author Haoyu Gu
     * @date 2016/10/20
     * @details
     * End Tests
     */
    void endTestCase();

    /**
     * @brief createNewSortOrder
     * @class QTTestNewSortOrder
     * @author Haoyu Gu
     * @date 2016/10/20
     * @details
     * Create new sort order, pass it to the UI componment and check.
     */
    void createNewSortOrder();
};

#endif // TEST_1_4_H
