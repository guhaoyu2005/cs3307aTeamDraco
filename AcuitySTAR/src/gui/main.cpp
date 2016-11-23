#include "gui/mainwindow.h"
#include "homepage.h"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

int main(int argc, char *argv[]) {
    int test = 0;   /* test on = 1 off = 0 */
    if (test) {
        //Initialize Test Classes
        QTTestCSVReader *test_csv = new QTTestCSVReader();
        QTTestMainWindow *test_MainWindow = new QTTestMainWindow();
        QTTestTreeModel *test_tree = new QTTestTreeModel();


        //Run Tests
        QTest::qExec(test_csv);
        QTest::qExec(test_tree);
        QTest::qExec(test_MainWindow);


        //Old CSV Test Now implemented as QTTestCSVReader
//        TestCSVReader testReader;
//        bool pass = testReader.CSV_READER_TEST_01();
//        if (pass) cout << "TEST_01 PASSED !\n";
//        else cout << "TEST_01 FAILED !\n";
//        pass = testReader.CSV_READER_TEST_02();
//        if (pass) cout << "TEST_02 PASSED !\n";
//        else cout << "TEST_02 FAILED !\n";
//        pass = testReader.CSV_READER_TEST_03();
//        if (pass) cout << "TEST_03 PASSED !\n";
//        else cout << "TEST_03 FAILED !\n";
//        pass = testReader.CSV_READER_TEST_04();
//        if (pass) cout << "TEST_04 PASSED !\n";
//        else cout << "TEST_04 FAILED !\n";
//        pass = testReader.CSV_READER_TEST_05();
//        if (pass) cout << "TEST_05 PASSED !\n";
//        else cout << "TEST_05 FAILED !\n";
//        pass = testReader.CSV_READER_TEST_06();
//        if (pass) cout << "TEST_06 PASSED !\n";
//        else cout << "TEST_06 FAILED !\n";
//        pass = testReader.CSV_READER_TEST_07();
//        if (pass) cout << "TEST_07 PASSED !\n";
//        else cout << "TEST_07 FAILED !\n";
//        pass = testReader.CSV_READER_TEST_08();
//        if (pass) cout << "TEST_08 PASSED !\n";
//        else cout << "TEST_08 FAILED !\n";
//        testReader.CSV_READER_TEST_ALL();
    }
#ifdef _WIN32
    FreeConsole();
#endif
    QApplication app(argc, argv);

    HomePage w;
    w.show();

    //UI testing must occurs after QApplication initialized.
    if (test) {
        QTTestNewSortOrder *test_SortOrder = new QTTestNewSortOrder();
        QTTestLoadFile *load_previous = new QTTestLoadFile();
        //QTest::qExec(test_SortOrder);
        //QTest::qExec(load_previous);
    }

    return app.exec();
}
