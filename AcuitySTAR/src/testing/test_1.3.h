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
     * @briefteachPDF
     * @class QTTestMainWindow
     * @author Michael Park
     * @date 2016/10/19
     * @details
     * Test case for handling invalid load on teach
     */
    void teachInvalidLoad();

    /**
     * @briefteachPDF
     * @class QTTestMainWindow
     * @author Michael Park
     * @date 2016/10/19
     * @details
     * Test case for handling invalid load on publication
     */
    void publicationInvalidLoad();

    /**
     * @briefteachPDF
     * @class QTTestMainWindow
     * @author Michael Park
     * @date 2016/10/19
     * @details
     * Test case for handling invalid load on presentation
     */
    void presentationInvalidLoad();

    /**
     * @briefteachPDF
     * @class QTTestMainWindow
     * @author Michael Park
     * @date 2016/10/19
     * @details
     * Test case for handling invalid load on fund/grant
     */
    void fundInvalidLoad();
};

#endif // TEST_1_3_H
