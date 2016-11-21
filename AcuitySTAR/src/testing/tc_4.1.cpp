#include "test_4.1.h"

using namespace std;

#define TEST_PREVIOUS_FILES_SAVE "testpreviousfiles.dat"

void QTTestLoadFile::initTestCase()
{
    qDebug("TC_4.1 Test Load Previous file Initialized");
    //mainTest = (MainWindow*) QApplication::activeWindow();
}

void QTTestLoadFile::readPreviousFile()
{
    MainWindow mainTest;
    QVERIFY(mainTest.load_serialized_paths(TEST_PREVIOUS_FILES_SAVE));
}

void QTTestLoadFile::serializeFiles()
{
    MainWindow mainTest;
    QVERIFY(mainTest.serialize_loaded_paths(TEST_PREVIOUS_FILES_SAVE));
}

void QTTestLoadFile::cleanupTestCase()
{
   qDebug("TC_4.1 Test Load Previous file Finished");
}
