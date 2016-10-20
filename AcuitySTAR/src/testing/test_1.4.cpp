#include "test_1.4.h"

#include "ui_mainwindow.h"
#include "./gui/mainwindow.h"

Ui::MainWindow* ui;
MainWindow* uiCtl;

void QTTestNewSortOrder::initTestCase() {
    qDebug("TEST_1.4 New Sort Order Test Initialized.");
    ui = new Ui::MainWindow();
    uiCtl = new MainWindow();
    ui->setupUi(uiCtl);
    createNewSortOrder();
}

void QTTestNewSortOrder::createNewSortOrder() {
    static std::vector<std::string> GRANTS_MANFIELDS, PRES_MANFIELDS, PUBS_MANFIELDS, TEACH_MANFIELDS;
    GRANTS_MANFIELDS = {"Member Name", "Funding Type", "Status", "Peer Reviewed?", "Role", "Title", "Start Date"};
    PRES_MANFIELDS = {"Member Name", "Date", "Type", "Role", "Title"};
    PUBS_MANFIELDS = {"Member Name", "Type", "Status Date", "Role", "Title"};
    TEACH_MANFIELDS = {"Member Name", "Start Date", "Program"};

    int oldCount;
    oldCount = ui->teach_sort->count();
    ui->teach_sort->addItem(QString::fromStdString(TEACH_MANFIELDS.at(0)));
    QVERIFY(oldCount+1==ui->teach_sort->count());

    oldCount = ui->pres_sort->count();
    ui->pres_sort->addItem(QString::fromStdString(PRES_MANFIELDS.at(0)));
    QVERIFY(oldCount+1==ui->pres_sort->count());

    oldCount = ui->pub_sort->count();
    ui->pub_sort->addItem(QString::fromStdString(PUBS_MANFIELDS.at(0)));
    QVERIFY(oldCount+1==ui->pub_sort->count());

    oldCount = ui->fund_sort->count();
    ui->fund_sort->addItem(QString::fromStdString(GRANTS_MANFIELDS.at(0)));
    QVERIFY(oldCount+1==ui->fund_sort->count());
}

void QTTestNewSortOrder::cleanupTestCase() {
    qDebug("TEST_1.4 New Sort Order Test Cleaning...");
    delete ui;
    delete uiCtl;
}

void QTTestNewSortOrder::endTestCase() {
    qDebug("TEST_1.4 New Sort Order Test Finished.");
}
