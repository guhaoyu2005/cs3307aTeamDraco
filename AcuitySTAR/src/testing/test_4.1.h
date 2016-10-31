#ifndef TC_4_1_H
#define TC_4_1_H

#include <string>
#include <vector>
#include <QObject>
#include <QDebug>
#include <QtTest/QtTest>

//Include for CSV Reader Testing
#include "../gui/mainwindow.h"

class MainWindow;
/**
 * @brief  TC_4.1 - The QTTestLoadFile class
 * @class QTTestLoadFile
 * @author Michael Park
 * @date 2016/10/31
 * @details Test case for loading previous files
 */
class QTTestLoadFile: public QObject
{
    Q_OBJECT
private slots:
    /**
     * @brief initTestCase
     * @class QTTestLoadFile
     * @author Michael Park
     * @date 2016/10/31
     * @details
     * Initialize the Test Case
     */
    void initTestCase();

    /**
     * @brief readPreviousFile
     * @class QTTestLoadFile
     * @author Michael Park
     * @date 2016/10/31
     * @details
     * read the saved file
     */
    void readPreviousFile();

    /**
     * @brief serializeFiles
     * @class QTTestLoadFile
     * @author Michael Park
     * @date 2016/10/31
     * @details
     * save up the current files to load
     */
    void serializeFiles();

    /**
     * @brief cleanupTestCase
     * @class QTTestLoadFile
     * @author Michael Park
     * @date 2016/10/31
     * @details
     * Cleanup Tests
     */
    void cleanupTestCase();
private:
    //MainWindow *mainTest;
};
#endif // TC_4_1_H
