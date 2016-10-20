#ifndef TEST_1_3_H
#define TEST_1_3_H

#include <string>
#include <vector>
#include <QObject>
#include <QDebug>
#include <QtTest/QtTest>

//Include for mainWindow Testing
#include "./gui/mainwindow.h"

/**
 * @brief The QTTestMainWindow class
 * @class QTTestCSVReader
 * @author Michael Park
 * @date 2016/10/19
 * @details
 * QT Test Class for mainwindow class
 */
class QTTestMainWindow: public QObject
{
    Q_OBJECT
private slots:
    /**
     * @brief initTestCase
     * @class QTTestMainWindow
     * @author Michael Park
     * @date 2016/10/19
     * @details
     * Telling that this test case is initialized
     */
    void initTestCase();

    /**
     * @brief teachInvalidLoad
     * @class QTTestMainWindow
     * @author Michael Park
     * @date 2016/10/19
     * @details
     * Test case for handling invalid load on teach
     */
    void teachInvalidLoad();

    /**
     * @brief publicationInvalidLoad
     * @class QTTestMainWindow
     * @author Michael Park
     * @date 2016/10/19
     * @details
     * Test case for handling invalid load on publication
     */
    void publicationInvalidLoad();

    /**
     * @brief presentationInvalidLoad
     * @class QTTestMainWindow
     * @author Michael Park
     * @date 2016/10/19
     * @details
     * Test case for handling invalid load on presentation
     */
    void presentationInvalidLoad();

    /**
     * @brief fundInvalidLoad
     * @class QTTestMainWindow
     * @author Michael Park
     * @date 2016/10/19
     * @details
     * Test case for handling invalid load on fund/grant
     */
    void fundInvalidLoad();

    /**
     * @brief endTestCase
     * @class QTTestMainWindow
     * @author Michael Park
     * @date 2016/10/19
     * @details
     * Function to tell this test case is finished
     */
    void endTestCase();
};

#endif // TEST_1_3_H
