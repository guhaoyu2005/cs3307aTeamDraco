#ifndef TEST_1_1_H
#define TEST_1_1_H

#include <string>
#include <vector>
#include <QObject>
#include <QDebug>
#include <QtTest/QtTest>

//Include for CSV Reader Testing
#include "./database/CSVReader.h"

/**
 * @brief TC_1.1 - The QTTestCSVReader class
 * @class QTTestCSVReader
 * @author James Wake
 * @date 2016/10/19
 * @details
 * TC_1.1 - QT Test Class for CSV Reader
 */
class QTTestCSVReader: public QObject
{
    Q_OBJECT
private slots:
    /**
     * @brief initTestCase
     * @class QTTestCSVReader
     * @author James Wake
     * @date 2016/10/19
     * @details
     * Initialize the Test Case
     */
    void initTestCase();

    /**
     * @brief QTTestCSVReader::readGrants
     * @class QTTestCSVReader
     * @author James Wake
     * @date 2016/10/19
     * @details
     * Read a Grants File and Verify non Empty Header
     */
    void readGrants();

    /**
     * @brief QTTestCSVReader::readGrantsClinical
     * @class QTTestCSVReader
     * @author James Wake
     * @date 2016/10/19
     * @details
     * Read a Grants Clinical File and Verify non Empty Header
     */
    void readGrantsClinical();

    /**
     * @brief QTTestCSVReader::readPresentations1
     * @class QTTestCSVReader
     * @author James Wake
     * @date 2016/10/19
     * @details
     * Read a Presentations File and Verify non Empty Header
     *
     */
    void readPresentations1();

    /**
     * @brief QTTestCSVReader::readPresentations2
     * @class QTTestCSVReader
     * @author James Wake
     * @date 2016/10/19
     * @details
     * Read a Presentations File and Verify non Empty Header
     */
    void readPresentations2();

    /**
     * @brief QTTestCSVReader::readTeaching
     * @class QTTestCSVReader
     * @author James Wake
     * @date 2016/10/19
     * @details
     * Read a Teaching File and Verify non Empty Header
     */
    void readTeaching();

    /**
     * @brief QTTestCSVReader::readNoFile
     * @class QTTestCSVReader
     * @author James Wake
     * @date 2016/10/19
     * @details
     * Read NO_FILE and Verify Empty Header
     */
    void readNoFile();

    /**
     * @brief QTTestCSVReader::dataPresentations
     * @class QTTestCSVReader
     * @author James Wake
     * @date 2016/10/19
     * @details
     * Check for data from reading Presentations
     */
    void dataPresentations();

    /**
     * @brief QTTestCSVReader::dataTeaching
     * @class QTTestCSVReader
     * @author James Wake
     * @date 2016/10/19
     * @details
     * Check for data from reading Teching
     */
    void dataTeaching();

    /**
     * @brief cleanupTestCase
     * @class QTTestCSVReader
     * @author James Wake
     * @date 2016/10/19
     * @details
     * Cleanup Tests
     */
    void cleanupTestCase();
};


#endif // TEST_1_1_H
