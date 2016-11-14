#include <QDate>
#include <QDebug>
#include <QDirModel>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QPainter>
#include <QPdfWriter>
#include <QPrintDialog>
#include <QPrinter>
#include <QString>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "CustomSort.h"
#include "ErrorEditDialog.h"

#include "database/CSVReader.h"
#include "database/QSortListIO.h"
#include "database/RecordsManager.h"
#include "datamodel/TreeModel.h"
#include "datamodel/GrantFundingTreeModel.h"
#include "datamodel/PresentationTreeModel.h"
#include "datamodel/PublicationTreeModel.h"
#include "datamodel/TeachingTreeModel.h"

#define FUNDORDER_SAVE  "fundsortorder.dat"
#define PRESORDER_SAVE  "pressortorder.dat"
#define PUBORDER_SAVE   "pubsortorder.dat"
#define TEACHORDER_SAVE "teachsortorder.dat"
//Persistence - Team Draco 2016
#define PREVIOUS_FILES_SAVE "previousfiles.dat"

std::vector<std::string> MainWindow::GRANTS_MANFIELDS = {"Member Name", "Funding Type", "Status", "Peer Reviewed?", "Role", "Title", "Start Date"};
std::vector<std::string> MainWindow::PRES_MANFIELDS = {"Member Name", "Date", "Type", "Role", "Title"};
std::vector<std::string> MainWindow::PUBS_MANFIELDS = {"Member Name", "Type", "Status Date", "Role", "Title"};
std::vector<std::string> MainWindow::TEACH_MANFIELDS = {"Member Name", "Start Date", "Program"};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow),
    fundTree(NULL), presTree(NULL), pubTree(NULL), teachTree(NULL),
    funddb(NULL), presdb(NULL), pubdb(NULL), teachdb(NULL) {
    // draw GUI elements
    ui->setupUi(this);

    this->showMaximized();

    // set up the logo
   // QPixmap logo(":/logo.png");
    //ui->logo_label->setPixmap(logo);

    // set up application icon
    QIcon icon(":/icon32.ico");
    MainWindow::setWindowIcon(icon);

    // set the date format
    QString dFormat("yyyy");
    ui->FromDate->setDisplayFormat(dFormat);
    ui->ToDate->setDisplayFormat(dFormat);

    // set start year to be 1950
    QDate startDate(1950, 1, 1);

    // set ranges for spinbox
    ui->FromDate->setDateRange(startDate, QDate::currentDate());
    ui->ToDate->setDateRange(startDate, QDate::currentDate());

    // set default dates (1950->current)
    ui->FromDate->setDate(startDate);
    ui->ToDate->setDate(QDate::currentDate());

    // set some member variables to the current date values
    yearStart = 1950;
    yearEnd = QDate::currentDate().year();

    //default print is disabled
    ui->teachPrintButton->setEnabled(false);
    ui->fundPrintButton->setEnabled(false);
    ui->pubPrintButton->setEnabled(false);
    ui->presPrintButton->setEnabled(false);

    ui->teachExportButton->setEnabled(false);
    ui->fundExportButton->setEnabled(false);
    ui->pubExportButton->setEnabled(false);
    ui->presExportButton->setEnabled(false);

    //Setup printer
    printer = new QPrinter();

    dateChanged = {false, false, false, false};

    //Load Serialized States - Team Draco 2016
    this->load_serialized_paths(PREVIOUS_FILES_SAVE);

}

MainWindow::~MainWindow() {
    delete ui;

    delete fundTree;
    delete presTree;
    delete pubTree;
    delete teachTree;

    delete funddb;
    delete presdb;
    delete pubdb;
    delete teachdb;
    delete printer;
}

void MainWindow::on_actionLoad_file_triggered() {
    QStringList filePaths = QFileDialog::getOpenFileNames(this,
                                                          "Select one or more files to load",
                                                          QDir::currentPath(),
                                                          tr("CSV (*.csv);; All files (*.*)"));
    if (filePaths.size() > 0) {
        const int NUM_TABS = 4;
        bool all_loaded[NUM_TABS] = {false, false, false, false};
        int sum = std::accumulate(all_loaded, all_loaded + NUM_TABS, 0);
        QStringList::Iterator it = filePaths.begin();
        while (sum != NUM_TABS && it != filePaths.end()) {
            QString path = it[0];
            //note short-circuit eval
            if (!all_loaded[FUNDING] && load_fund(path, true)) {
                all_loaded[FUNDING] = true;
            } else if (!all_loaded[TEACH] && load_teach(path, true)) {
                all_loaded[TEACH] = true;
            } else if (!all_loaded[PUBLICATIONS] && load_pub(path, true)) {
                all_loaded[PUBLICATIONS] = true;
            } else if (!all_loaded[PRESENTATIONS] && load_pres(path, true)) {
                all_loaded[PRESENTATIONS] = true;
            }
            sum = std::accumulate(all_loaded, all_loaded + NUM_TABS, 0);
            ++it;
        }
    }
}


/*
 * When the load file button is clicked a file open dialog box
 * is opened.  If a file name is successfully returned, makeTree()
 * is called.
 */
QString MainWindow::load_file() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath(),
                                                    tr("CSV (*.csv);; All files (*.*)"));

    if (!filePath.isEmpty()) {
        return filePath;
    } else {
        return "";
    }
}

bool MainWindow::checkTargetTabHasFileLoaded(TABS tabId) {
    switch (tabId) {
    case TEACH: {
        return teachdb?true:false;
    }
    case PRESENTATIONS: {
        return presdb?true:false;
    }
    case PUBLICATIONS: {
        return pubdb?true:false;
    }
    case FUNDING: {
        return funddb?true:false;
    }
    }
}

void MainWindow::loadFileAndSwitchToProperTab() {
    QString filePath = load_file();
    if (!filePath.isEmpty()) {
        CSVReader reader;
        reader.loadCSV(filePath.toStdString());
        std::string errMsg;
        if (reader.getFileType(errMsg) == CSVReader::CSVFileTypeBadFile) {
            QMessageBox::StandardButton badFileMsgbox;
            badFileMsgbox = QMessageBox::question(this, "Bad file", "Failed to load file or file format invalid. Retry?", QMessageBox::Yes|QMessageBox::No);
            if (badFileMsgbox == QMessageBox::Yes) {
                loadFileAndSwitchToProperTab();
                return;
            }
            else {
                //do nothing and return
                return;
            }
        }
        else {
            switch (reader.getFileType(errMsg)) {
                case CSVReader::CSVFileTypeTeaching: {
                    if (checkTargetTabHasFileLoaded(TEACH)) {
                        QMessageBox::StandardButton msgbox;
                        msgbox = QMessageBox::question(this, "Info", "Already loaded teach file, continue? \nAll unsaved work will lose.", QMessageBox::Yes|QMessageBox::No);
                        if (msgbox == QMessageBox::No) {
                            //do nothing and return
                            return;
                        }
                    }
                    //continue
                    load_teach(filePath);
                    ui->categoryTab->setCurrentIndex(0);
                    break;
                }
                case CSVReader::CSVFileTypePublications: {
                    if (checkTargetTabHasFileLoaded(PUBLICATIONS)) {
                        QMessageBox::StandardButton msgbox;
                        msgbox = QMessageBox::question(this, "Info", "Already loaded publication file, continue? \nAll unsaved work will lose.", QMessageBox::Yes|QMessageBox::No);
                        if (msgbox == QMessageBox::No) {
                            //do nothing and return
                            return;
                        }
                    }
                    load_pub(filePath);
                    ui->categoryTab->setCurrentIndex(1);
                    break;
                }
                case CSVReader::CSVFileTypePresentations: {
                    if (checkTargetTabHasFileLoaded(PRESENTATIONS)) {
                        QMessageBox::StandardButton msgbox;
                        msgbox = QMessageBox::question(this, "Info", "Already loaded presentation file, continue? \nAll unsaved work will lose.", QMessageBox::Yes|QMessageBox::No);
                        if (msgbox == QMessageBox::No) {
                            //do nothing and return
                            return;
                        }
                    }
                    load_pres(filePath);
                    ui->categoryTab->setCurrentIndex(2);
                    break;
                }
                case CSVReader::CSVFileTypeGrants: {
                    if (checkTargetTabHasFileLoaded(FUNDING)) {
                        QMessageBox::StandardButton msgbox;
                        msgbox = QMessageBox::question(this, "Info", "Already loaded funding file, continue? \nAll unsaved work will lose.", QMessageBox::Yes|QMessageBox::No);
                        if (msgbox == QMessageBox::No) {
                            //do nothing and return
                            return;
                        }
                    }
                    load_fund(filePath);
                    ui->categoryTab->setCurrentIndex(3);
                    break;
                }
            }
        }
    }
    else {
        //do nothing for now if didn't select any
    }
}
void MainWindow::refresh(int tabIndex) {
    // if we've loaded in a file, update that data
    switch (tabIndex) {
    case FUNDING:
        if (!fundPath.isEmpty()) {
            makeTree(FUNDING);
            ui->fund_graph_stackedWidget->hide();
            ui->fundGraphTitle->clear();
        }
        break;

    case PRESENTATIONS:
        if (!presPath.isEmpty()) {
            makeTree(PRESENTATIONS);
            ui->pres_graph_stackedWidget->hide();
            ui->presGraphTitle->clear();
        }
        break;
    case PUBLICATIONS:
        if (!pubPath.isEmpty()) {
            makeTree(PUBLICATIONS);
            ui->pub_graph_stackedWidget->hide();
            ui->pubGraphTitle->clear();
        }
        break;

    case TEACH:
        if (!teachPath.isEmpty()) {
            makeTree(TEACH);
            ui->teach_graph_stackedWidget->hide();
            ui->teachGraphTitle->clear();
        }
        break;
    }
}

int MainWindow::checkFile(int index, QString filePath) {
    CSVReader reader;
    std::vector<std::string> header;
    std::string searchstring;

    int sortHeaderIndex = 2;

    switch (index) {

    case TEACH:
        // read if first or if a new file is loaded
        if (teachPath.isEmpty() || (!teachPath.isEmpty() && filePath.compare(teachPath))) {
            // create a new reader to read in the file
            reader = CSVReader(filePath.toStdString());
            header = reader.getHeaders();

            // create a new manager for the data
            delete teachdb;
            teachdb = new RecordsManager(&header);

            // check for right file type by searching for unique header
            searchstring = "Program";
            if (std::find(header.begin(), header.end(), searchstring) != header.end()) {
                // load in data into the manager, with the date as the key
                sortHeaderIndex = teachdb->getHeaderIndex("Start Date");
                teachData = reader.getData();
                std::vector<std::vector<std::string>*> f_errs;
                unsigned int j;
                for (int i = 0; i < (int) teachData.size(); i++) {
                    for (j = 0; j < TEACH_MANFIELDS.size(); j++) {
                        int index = teachdb->getHeaderIndex(TEACH_MANFIELDS[j]);
                        if (teachData[i][index].compare("") == 0) {
                            f_errs.push_back(&teachData[i]);
                            break;
                        }
                    }

                    // if all mandatory fields are okay
                    if (j == TEACH_MANFIELDS.size()) {
                        // date interpretation
                        int yrIndex = teachdb->getHeaderIndex("Start Date");
                        int year;
                        sscanf(teachData[i][yrIndex].c_str(), "%4d", &year);
                        teachData[i][yrIndex] = std::to_string(year);

                        teachdb->addRecord(reader.parseDateString(teachData[i][sortHeaderIndex]), &teachData[i]);
                    }
                }

                if (f_errs.size() > 0) {
                    if(handle_field_errors(f_errs, header, TEACH_MANFIELDS, filePath, TEACH)) {
                        for (unsigned int i = 0; i < f_errs.size(); i++) {
                            teachdb->addRecord(reader.parseDateString((*(f_errs[i]))[sortHeaderIndex]), f_errs[i]);
                        }
                    }
                }
            } else {
                return EXIT_FAILURE;
            }
        } else {
            return EXIT_SUCCESS;
        }
        ui->teachPrintButton->setEnabled(true);
        ui->teachExportButton->setEnabled(true);
        break;

    case PUBLICATIONS:
        // read if first or if a new file is loaded
        if (presPath.isEmpty() || (!presPath.isEmpty() && filePath.compare(presPath))) {
            // create a new reader to read in the file
            reader = CSVReader(filePath.toStdString());
            header = reader.getHeaders();

            // create a new manager for the data
            delete pubdb;
            pubdb = new RecordsManager(&header);

            // check for right file type by searching for unique header
            searchstring = "Publication Status";
            if (std::find(header.begin(), header.end(), searchstring) != header.end()) {
                // load in data into the manager, with the date as the key
                sortHeaderIndex = pubdb->getHeaderIndex("Status Date");
                pubData = reader.getData();
                std::vector<std::vector<std::string>*> f_errs;
                unsigned int j;
                for (int i = 0; i < (int) pubData.size(); i++) {
                    for (j = 0; j < PUBS_MANFIELDS.size(); j++) {
                        int index = pubdb->getHeaderIndex(PUBS_MANFIELDS[j]);
                        if (pubData[i][index].compare("") == 0) {
                            f_errs.push_back(&pubData[i]);
                            break;
                        }
                    }

                    // if all mandatory fields are okay
                    if (j == PUBS_MANFIELDS.size()) {
                        // date interpretation
                        int yrIndex = pubdb->getHeaderIndex("Status Date");
                        int year;
                        sscanf(pubData[i][yrIndex].c_str(), "%4d", &year);
                        pubData[i][yrIndex] = std::to_string(year);

                        pubdb->addRecord(reader.parseDateString(pubData[i][sortHeaderIndex]), &pubData[i]);
                    }
                }

                if (f_errs.size() > 0) {
                    if(handle_field_errors(f_errs, header, PUBS_MANFIELDS, filePath, PUBLICATIONS)) {
                        for (unsigned int i = 0; i < f_errs.size(); i++) {
                            pubdb->addRecord(reader.parseDateString((*(f_errs[i]))[sortHeaderIndex]), f_errs[i]);
                        }
                    }
                }
            } else {
                return EXIT_FAILURE;
            }
        } else {
            return EXIT_SUCCESS;
        }
        ui->pubPrintButton->setEnabled(true);
        ui->pubExportButton->setEnabled(true);
        break;

    case PRESENTATIONS:
        // read if first or if a new file is loaded
        if (presPath.isEmpty() || (!presPath.isEmpty() && filePath.compare(presPath))) {
            // create a new reader to read in the file
            reader = CSVReader(filePath.toStdString());
            header = reader.getHeaders();

            // create a new manager for the data
            delete presdb;
            presdb = new RecordsManager(&header);

            // check for right file type by searching for unique header
            searchstring = "Activity Type";
            if (std::find(header.begin(), header.end(), searchstring) != header.end()) {
                // load in data into the manager, with the date as the key
                sortHeaderIndex = presdb->getHeaderIndex("Date");
                presData = reader.getData();
                std::vector<std::vector<std::string>*> f_errs;
                unsigned int j = 0;
                for (int i = 0; i < (int) presData.size(); i++) {
                    //check through mandatory fields for empty fields
                    for (j = 0; j < PRES_MANFIELDS.size(); j++) {
                        int index = presdb->getHeaderIndex(PRES_MANFIELDS[j]);
                        if (presData[i][index].compare("") == 0) {
                            f_errs.push_back(&presData[i]);
                            break;
                        }
                    }

                    // if all mandatory fields are okay
                    if (j == PRES_MANFIELDS.size()) {
                        // date interpretation
                        int yrIndex = presdb->getHeaderIndex("Date");
                        int year;
                        sscanf(presData[i][yrIndex].c_str(), "%4d", &year);
                        presData[i][yrIndex] = std::to_string(year);

                        presdb->addRecord(reader.parseDateString(presData[i][sortHeaderIndex]), &presData[i]);
                    }
                }

                if (f_errs.size() > 0) {
                    if(handle_field_errors(f_errs, header, PRES_MANFIELDS, filePath, PRESENTATIONS)) {
                        for (unsigned int i = 0; i < f_errs.size(); i++) {
                            presdb->addRecord(reader.parseDateString((*(f_errs[i]))[sortHeaderIndex]), f_errs[i]);
                        }
                    }
                }
            } else {
                return EXIT_FAILURE;
            }
        } else {
            return EXIT_SUCCESS;
        }
        ui->presPrintButton->setEnabled(true);
        ui->presExportButton->setEnabled(true);
        break;

    case FUNDING:
        // read if first or if a new file is loaded
        if (fundPath.isEmpty() || (!fundPath.isEmpty() && filePath.compare(fundPath))) {
            // create a new reader to read in the file
            reader = CSVReader(filePath.toStdString());
            header = reader.getHeaders();

            // create a new manager for the data
            delete funddb;
            funddb = new RecordsManager(&header);

            // check for right file type by searching for unique header
            searchstring = "Funding Type";

            if (std::find(header.begin(), header.end(), searchstring) != header.end()) {
                // load in data into the manager, with the date as the key
                sortHeaderIndex = funddb->getHeaderIndex("Start Date");
                fundData = reader.getData();
                std::vector<std::vector<std::string>*> f_errs;
                unsigned int j;
                for (int i = 0; i < (int) fundData.size(); i++) {
                    for (j = 0; j < GRANTS_MANFIELDS.size(); j++) {
                        int index = funddb->getHeaderIndex(GRANTS_MANFIELDS[j]);
                        if (fundData[i][index].compare("") == 0) {
                            f_errs.push_back(&fundData[i]);
                            break;
                        }
                    }

                    // if all mandatory fields are okay
                    if (j == GRANTS_MANFIELDS.size()) {
                        // date interpretation
                        int yrIndex = funddb->getHeaderIndex("Start Date");
                        int year;
                        sscanf(fundData[i][yrIndex].c_str(), "%4d", &year);
                        fundData[i][yrIndex] = std::to_string(year);

                        // boolean interpretation
                        int prIndex = funddb->getHeaderIndex("Peer Reviewed?");
                        if (fundData[i][prIndex].compare("True") == 0) {
                            fundData[i][prIndex] = "Peer Reviewed";
                        } else {
                            fundData[i][prIndex] = "Not Peer Reviewed";
                        }
                        funddb->addRecord(reader.parseDateString(fundData[i][sortHeaderIndex]), &fundData[i]);
                    }
                }
                if (f_errs.size() > 0) {
                    if(handle_field_errors(f_errs, header, GRANTS_MANFIELDS, filePath, FUNDING)) {
                        for (unsigned int i = 0; i < f_errs.size(); i++) {
                            funddb->addRecord(reader.parseDateString((*(f_errs[i]))[sortHeaderIndex]), f_errs[i]);
                        }
                    }
                }
            } else {
                return EXIT_FAILURE;
            }
        } else {
            return EXIT_SUCCESS;
        }
        ui->fundPrintButton->setEnabled(true);
        ui->fundExportButton->setEnabled(true);
        break;
    }
    return EXIT_SUCCESS;
}

void MainWindow::createDefaultSortOrder(int tabIndex) {
    QStringList defaultOrder;
    defaultOrder << "Default";

    switch (tabIndex) {
    case FUNDING:
        // specify default sort order
        defaultOrder << "Member Name" << "Funding Type" << "Peer Reviewed?" << "Status" << "Role" << "Title";

        // add default list to member variable
        allFundOrders << defaultOrder;

    {
        // save the default for the user
        QSortListIO saveFundSort(FUNDORDER_SAVE);
        saveFundSort.saveList(allFundOrders);
    }

        break;
    case PRESENTATIONS:
        // specify default sort order
        defaultOrder << "Member Name" << "Type" << "Role" << "Title";

        // add default list to member variable
        allPresOrders << defaultOrder;

    {
        // save the default for the user
        QSortListIO savePresSort(PRESORDER_SAVE);
        savePresSort.saveList(allPresOrders);
    }

        break;
    case PUBLICATIONS:
        // specify default sort order
        defaultOrder << "Member Name" << "Type" << "Role" << "Title";

        // add default list to member variable
        allPubOrders << defaultOrder;

    {
        // save the default for the user
        QSortListIO savePubSort(PUBORDER_SAVE);
        savePubSort.saveList(allPubOrders);
    }

        break;
    case TEACH:
        // specify default sort order
        defaultOrder << "Member Name" << "Program";

        // add default list to member variable
        allTeachOrders << defaultOrder;

    {
        // save the default for the user
        QSortListIO saveTeachSort(FUNDORDER_SAVE);
        saveTeachSort.saveList(allFundOrders);
    }

        break;
    }
}

/*
 * err: vector of pointers to record entries.
 * headers: vector of strings containing the db headers
 * mandatory: vector of strings containing the mandatory fields in the db
 *
 * Returns true when the passed err vector contains edited fields.
 * Returns false when no fields have been edited.
 */
bool MainWindow::handle_field_errors(std::vector<std::vector<std::string>*>& err,
                                     std::vector<std::string>& headers,
                                     std::vector<std::string>& mandatory,
                                     QString filePath,
                                     TABS tab) {
    //Since CSVReader alldata contains completely empty records
    //remove them first.
    std::vector<std::vector<std::string>*>::iterator it;
    for (it = err.begin(); it != err.end(); it++) {
        bool allEmpty = true;
        for (int col = 0; col < (int) (*it)->size(); col++) {
            if ((*it)->at(col).compare("") != 0) {
                allEmpty = false;
            }
        }
        if (allEmpty) {
            it = err.erase(it);
            it--;
        }
    }
    //Return false; there are no errors to correct
    if (err.size() == 0) {
        return false;
    }
    QMessageBox prompt;
    QString mainText = "";
    switch(tab){
    case TEACH:
        mainText.append("Teaching File: ");
        break;
    case PUBLICATIONS:
        mainText.append("Publications File: ");
        break;
    case PRESENTATIONS:
        mainText.append("Presentations File: ");
        break;
    case FUNDING:
        mainText.append("Funding File: ");
        break;
    }
    mainText.append(filePath);
    mainText.append(" contains ");
    mainText.append(QString::number(err.size()));
    mainText.append(" records with missing mandatory fields.");
    prompt.setText(mainText);
    prompt.setInformativeText("Do you want to edit these entries or discard?");
    prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    prompt.setDefaultButton(QMessageBox::Yes);
    prompt.setButtonText(QMessageBox::Yes, "Edit");
    prompt.setButtonText(QMessageBox::No, "Discard");
    prompt.setWindowIcon(QIcon(":/icon32.ico"));
    int ret = prompt.exec();

    switch (ret) {
    case QMessageBox::Yes: {
        ErrorEditDialog diag(this, err, headers, mandatory);
        if(diag.exec()) {
            return true;
        }
        return false;
    }

    case QMessageBox::No:
    default:
        return false;
    }
}

/*
 * Builds a TreeModel for the ui's embedded treeview.
 */
void MainWindow::makeTree(int tabIndex) {
    switch (tabIndex) {
    case TEACH:
        // set up some member variables to point to the current data in use
        currentdb = teachdb;
        delete teachTree;
        teachTree = new TeachingTreeModel(currentdb);
        currentTree = teachTree;

        // get some UI elements so we can put the tree and graphs into them
        currentView = ui->teachTreeView;

        // analyze the data into a tree
        currentTree->setupModel(yearStart, yearEnd, teachSortOrder, getFilterStartChar(TEACH), getFilterEndChar(TEACH));

        ui->teach_pie_button->toggle();

        break;

    case PUBLICATIONS:
        // set up some member variables to point to the current data in use
        currentdb = pubdb;
        delete pubTree;
        pubTree = new PublicationTreeModel(currentdb);
        currentTree = pubTree;

        // get some UI elements so we can put the tree and graphs into them
        currentView = ui->pubTreeView;

        // analyze the data into a tree
        currentTree->setupModel(yearStart, yearEnd, pubSortOrder, getFilterStartChar(PUBLICATIONS), getFilterEndChar(PUBLICATIONS));

        ui->pub_pie_button->toggle();

        break;
    case PRESENTATIONS:
        // set up some member variables to point to the current data in use
        currentdb = presdb;
        delete presTree;

        presTree = new PresentationTreeModel(currentdb);
        currentTree = presTree;

        // get some UI elements so we can put the tree and graphs into them
        currentView = ui->presTreeView;

        // analyze the data into a tree
        currentTree->setupModel(yearStart, yearEnd, presSortOrder, getFilterStartChar(PRESENTATIONS), getFilterEndChar(PRESENTATIONS));

        ui->pres_pie_button->toggle();

        break;

    case FUNDING:
        // set up some member variables to point to the current data in use
        currentdb = funddb;
        delete fundTree;
        fundTree = new GrantFundingTreeModel(currentdb);
        currentTree = fundTree;

        // get some UI elements so we can put the tree and graphs into them
        currentView = ui->fundTreeView;

        // analyze the data into a tree
        currentTree->setupModel(yearStart, yearEnd, fundSortOrder, getFilterStartChar(FUNDING), getFilterEndChar(FUNDING));

        ui->fund_pie_button->toggle();

        break;
    }

    // set up the tree in our UI
    currentView->setModel(currentTree);

    // set resize property to stretch
    currentView->header()->resizeSections(QHeaderView::Stretch);
}

void MainWindow::setupPieChart(PieChartWidget* pieChart, QListWidget *pieListWidget, std::vector<std::pair <std::string, double>> pieChartList) {
    // draws the pie graph by sending piechartwidget a vector of name, presentation count
    int pieSize = (int) pieChartList.size();
    QVector<QColor> colorList(pieSize);
    pieListWidget->clear();
    for (int i = 0; i < pieSize; i++) {
        colorList[i] = (QColor(qrand() % 256, qrand() % 256, qrand() % 256));
        pieListWidget->addItem(QString::fromStdString(pieChartList[i].first));

        // set legend colors
        QPixmap pixmap(100, 100);
        pixmap.fill(QColor(colorList[i]));
        QIcon tempIcon(pixmap);
        pieListWidget->item(i)->setIcon(tempIcon);
    }

    pieChart->setData(pieChartList, colorList); //passes vector list to piechartwidget
}

void MainWindow::setupBarChart(QCustomPlot *barChart, std::vector<std::pair <std::string, double>> barChartList) {
    // create empty bar chart objects:
    QCPBars *yLabels = new QCPBars(barChart->yAxis, barChart->xAxis);
    barChart->addPlottable(yLabels);

    // set names and colors:
    QPen pen;
    pen.setWidthF(1.2);
    yLabels->setName("Type");
    pen.setColor(QColor(255, 131, 0));
    yLabels->setPen(pen);
    yLabels->setBrush(QColor(255, 131, 0, 50));

    //get label list
    int barSize = (int) barChartList.size();
    double maxCount = 0;
    double scaledCount;
    QVector<double> ticks;
    QVector<QString> ylabels;
    QVector<double> count;

    //add label list to y axis labels
    for (int i = 0; i < barSize; i++){
        ticks << (i+1);
        ylabels << QString::fromStdString(barChartList[i].first);
        if (barChartList[i].second>1000000){
            scaledCount = barChartList[i].second/1000000;
        } else if (barChartList[i].second>1000){
            scaledCount = barChartList[i].second/1000;
        } else{
            scaledCount = barChartList[i].second;
        }
        count <<scaledCount;

        if (maxCount < barChartList[i].second)
            maxCount = barChartList[i].second;
    }

    //setup Y Axis
    barChart->yAxis->setAutoTicks(false);
    barChart->yAxis->setAutoTickLabels(false);
    barChart->yAxis->setTickVector(ticks);
    barChart->yAxis->setTickVectorLabels(ylabels);
    barChart->yAxis->setTickLabelPadding(1);
    barChart->yAxis->setSubTickCount(0);
    barChart->yAxis->setTickLength(0, 1);
    barChart->yAxis->grid()->setVisible(true);
    barChart->yAxis->setRange(0, barSize+1);

    if(maxCount>1000000){
        maxCount = maxCount/1000000;
        barChart->xAxis->setLabel("Total (in Millions)");
    }else if (maxCount>1000){
        maxCount = maxCount/1000;
        barChart->xAxis->setLabel("Total (in Thousands)");
    }else{
        barChart->xAxis->setLabel("Total");
    }

    // setup X Axis
    barChart->xAxis->setAutoTicks(true);
    barChart->xAxis->setRange(0,maxCount+(maxCount*.05));
    barChart->xAxis->setAutoTickLabels(true);
    barChart->xAxis->setAutoTickStep(true);
    barChart->xAxis->grid()->setSubGridVisible(true);

    QPen gridPen;
    gridPen.setStyle(Qt::SolidLine);
    gridPen.setColor(QColor(0, 0, 0, 25));
    barChart->xAxis->grid()->setPen(gridPen);
    gridPen.setStyle(Qt::DotLine);
    barChart->xAxis->grid()->setSubGridPen(gridPen);

    yLabels->setData(ticks, count);
}

//set up for the Box Plot (added new chart)
void MainWindow::setupBoxPlot(QCustomPlot *boxPlot, std::vector<std::pair <std::string, double>> boxPlotList){
    QCPStatisticalBox *statistical = new QCPStatisticalBox(boxPlot->xAxis, boxPlot->yAxis);
    QBrush boxBrush(QColor(60, 60, 255, 100));
    boxBrush.setStyle(Qt::Dense6Pattern); // make it look oldschool
    statistical->setBrush(boxBrush);

    //setData(double key, double minimum, double lowerQuartile, double median, double upperQuartile, double maximum)
    int boxSize = (int) boxPlotList.size();
    double maxCount = 0;
    double scaledCount;
    QVector<double> ticks;
    QVector<QString> xlabels;
    QVector<double> count;
    for (int i = 0; i < boxSize; i++){
        ticks << (i+1);
        //getting x labels
        xlabels << QString::fromStdString(boxPlotList[i].first);
        // setting the data with values
        if (boxPlotList[i].second>1000000){
            scaledCount = boxPlotList[i].second/1000000;
        } else if (boxPlotList[i].second>1000){
            scaledCount = boxPlotList[i].second/1000;
        } else{
            scaledCount = boxPlotList[i].second;
        }
        count <<scaledCount;

        if (maxCount < boxPlotList[i].second)
            maxCount = boxPlotList[i].second;
    }

    //add label list to x axis labels
    if(maxCount>1000000){
        maxCount = maxCount/1000000;
        boxPlot->yAxis->setLabel("Total (in Millions)");
    }else if (maxCount>1000){
        maxCount = maxCount/1000;
        boxPlot->yAxis->setLabel("Total (in Thousands)");
    }else{
        boxPlot->yAxis->setLabel("Total");
    }


    //setup X Axis
    boxPlot->xAxis->setAutoTicks(false);
    boxPlot->xAxis->setAutoTickLabels(false);
    boxPlot->xAxis->setTickVector(ticks);
    boxPlot->xAxis->setTickVectorLabels(xlabels);
    boxPlot->xAxis->setTickLabelPadding(1);
    boxPlot->xAxis->setSubTickCount(0);
    boxPlot->xAxis->setTickLength(0, 1);
    boxPlot->xAxis->grid()->setVisible(true);
    boxPlot->xAxis->setRange(0, boxSize+1);

    // setup Y Axis
    boxPlot->yAxis->setAutoTicks(true);
    boxPlot->yAxis->setRange(0,maxCount+(maxCount*.05));
    boxPlot->yAxis->setAutoTickLabels(true);
    boxPlot->yAxis->setAutoTickStep(true);
    boxPlot->yAxis->grid()->setSubGridVisible(true);

    /*
    // prepare axes:
    boxPlot->rescaleAxes();
    boxPlot->xAxis->setTickVectorLabels(xlabels);
    //boxPlot->xAxis->scaleRange(1.7, boxPlot->xAxis->range().center());
    boxPlot->yAxis->setRange(0, 7);*/
    boxPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

}

void MainWindow::on_teach_new_sort_clicked() {
    if (teachdb != NULL) {
        CustomSort* sortdialog = new CustomSort();
        sortdialog->setFields(TEACH_MANFIELDS);
        int ret = sortdialog->exec();
        if (ret) {
            QStringList newSortOrder = sortdialog->getSortFields();
            allTeachOrders << newSortOrder;
            ui->teach_sort->addItem(newSortOrder.at(0));

            // save the sort fields to file
            QSortListIO saveTeachSort(TEACHORDER_SAVE);
            saveTeachSort.saveList(allTeachOrders);
        }
        delete sortdialog;
    } else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}

void MainWindow::on_pub_new_sort_clicked() {
    if (pubdb != NULL) {
        CustomSort* sortdialog = new CustomSort();
        sortdialog->setFields(PUBS_MANFIELDS);
        int ret = sortdialog->exec();
        if (ret) {
            QStringList newSortOrder = sortdialog->getSortFields();
            allPubOrders << newSortOrder;
            ui->pub_sort->addItem(newSortOrder.at(0));

            // save the sort fields to file
            QSortListIO savePubSort(PUBORDER_SAVE);
            savePubSort.saveList(allPubOrders);
        }
        delete sortdialog;
    } else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}

void MainWindow::on_pres_new_sort_clicked() {
    if (presdb != NULL) {
        CustomSort* sortdialog = new CustomSort();
        sortdialog->setFields(PRES_MANFIELDS);
        int ret = sortdialog->exec();
        if (ret) {
            QStringList newSortOrder = sortdialog->getSortFields();
            allPresOrders << newSortOrder;
            ui->pres_sort->addItem(newSortOrder.at(0));

            // save the sort fields to file
            QSortListIO savePresSort(PRESORDER_SAVE);
            savePresSort.saveList(allPresOrders);
        }
        delete sortdialog;
    } else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}

void MainWindow::on_fund_new_sort_clicked() {
    if (funddb != NULL) {
        CustomSort* sortdialog = new CustomSort();
        sortdialog->setFields(GRANTS_MANFIELDS);
        int ret = sortdialog->exec();
        if (ret) {
            QStringList newSortOrder = sortdialog->getSortFields();
            allFundOrders << newSortOrder;
            ui->fund_sort->addItem(newSortOrder.at(0));

            // save the sort fields to file
            QSortListIO saveFundSort(FUNDORDER_SAVE);
            saveFundSort.saveList(allFundOrders);
        }
        delete sortdialog;
    } else {
        QMessageBox::critical(this, "Missing File", "Please load a file first.");
    }
}

void MainWindow::on_teach_sort_currentIndexChanged(int index) {
    if(index != -1) {
        QStringList sortOrder = allTeachOrders[index];
        teachSortOrder.clear();
        for (int i = 1; i < sortOrder.size(); i++) {
            teachSortOrder.emplace_back(sortOrder[i].toStdString());
        }
        ui->teach_filter->setText(QString::fromStdString(teachSortOrder[0]));
        refresh(TEACH);
    }
}

void MainWindow::on_pub_sort_currentIndexChanged(int index) {
    if(index != -1) {
        QStringList sortOrder = allPubOrders[index];
        pubSortOrder.clear();
        for (int i = 1; i < sortOrder.size(); i++) {
            pubSortOrder.emplace_back(sortOrder[i].toStdString());
        }
        ui->pub_filter->setText(QString::fromStdString(pubSortOrder[0]));
        refresh(PUBLICATIONS);
    }
}

void MainWindow::on_pres_sort_currentIndexChanged(int index) {
    if(index != -1) {
        QStringList sortOrder = allPresOrders[index];
        presSortOrder.clear();
        for (int i = 1; i < sortOrder.size(); i++) {
            presSortOrder.emplace_back(sortOrder[i].toStdString());
        }
        ui->pres_filter->setText(QString::fromStdString(presSortOrder[0]));
        refresh(PRESENTATIONS);
    }
}

void MainWindow::on_fund_sort_currentIndexChanged(int index) {
    if(index != -1) {
        QStringList sortOrder = allFundOrders[index];
        fundSortOrder.clear();
        for (int i = 1; i < sortOrder.size(); i++) {
            fundSortOrder.emplace_back(sortOrder[i].toStdString());
        }
        ui->fund_filter->setText(QString::fromStdString(fundSortOrder[0]));
        refresh(FUNDING);
    }
}

void MainWindow::on_teach_delete_sort_clicked() {
    if (ui->teach_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->teach_sort->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (prompt.exec()==QMessageBox::Yes) {
            allTeachOrders.removeAt(ui->teach_sort->currentIndex());
            ui->teach_sort->removeItem(ui->teach_sort->currentIndex());

            QSortListIO saveTeachSort(TEACHORDER_SAVE);
            saveTeachSort.saveList(allTeachOrders);
        }
    } else {
        QMessageBox::critical(this, "", "Cannot delete Default");
    }
}

void MainWindow::on_pub_delete_sort_clicked() {
    if (ui->pub_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->pub_sort->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (prompt.exec()==QMessageBox::Yes) {
            allPubOrders.removeAt(ui->pub_sort->currentIndex());
            ui->pub_sort->removeItem(ui->pub_sort->currentIndex());

            QSortListIO savePubSort(PUBORDER_SAVE);
            savePubSort.saveList(allPubOrders);
        }
    } else {
        QMessageBox::critical(this, "", "Cannot delete Default");
    }
}

void MainWindow::on_pres_delete_sort_clicked() {
    if (ui->pres_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->pres_sort->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (prompt.exec()==QMessageBox::Yes) {
            allPresOrders.removeAt(ui->pres_sort->currentIndex());
            ui->pres_sort->removeItem(ui->pres_sort->currentIndex());

            QSortListIO savePresSort(PRESORDER_SAVE);
            savePresSort.saveList(allPresOrders);
        }
    } else {
        QMessageBox::critical(this, "", "Cannot delete Default");
    }
}

void MainWindow::on_fund_delete_sort_clicked() {
    if (ui->fund_sort->currentIndex()!=0) {
        QMessageBox prompt;
        prompt.setText("Are you sure you want to delete " + ui->fund_sort->currentText() + "?");
        prompt.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (prompt.exec()==QMessageBox::Yes) {
            allFundOrders.removeAt(ui->fund_sort->currentIndex());
            ui->fund_sort->removeItem(ui->fund_sort->currentIndex());

            QSortListIO saveFundSort(FUNDORDER_SAVE);
            saveFundSort.saveList(allFundOrders);
        }
    } else {
        QMessageBox::critical(this, "", "Cannot delete Default");
    }
}

void MainWindow::on_teach_bar_button_toggled() { ui->teach_graph_stackedWidget->setCurrentIndex(2);}
void MainWindow::on_teach_pie_button_toggled() { ui->teach_graph_stackedWidget->setCurrentIndex(1);}
void MainWindow::on_teach_stat_button_toggled() { ui->teach_graph_stackedWidget->setCurrentIndex(0);}
void MainWindow::on_pub_bar_button_toggled() { ui->pub_graph_stackedWidget->setCurrentIndex(2);}
void MainWindow::on_pub_pie_button_toggled() { ui->pub_graph_stackedWidget->setCurrentIndex(0);}
void MainWindow::on_pub_stat_button_toggled() { ui->pub_graph_stackedWidget->setCurrentIndex(1);}
void MainWindow::on_pres_bar_button_toggled() { ui->pres_graph_stackedWidget->setCurrentIndex(2);}
void MainWindow::on_pres_pie_button_toggled() { ui->pres_graph_stackedWidget->setCurrentIndex(0);}
void MainWindow::on_pres_stat_button_toggled() { ui->pres_graph_stackedWidget->setCurrentIndex(1);}
void MainWindow::on_fund_bar_button_toggled() { ui->fund_graph_stackedWidget->setCurrentIndex(2);}
void MainWindow::on_fund_pie_button_toggled() { ui->fund_graph_stackedWidget->setCurrentIndex(0);}
void MainWindow::on_fund_stat_button_toggled() { ui->fund_graph_stackedWidget->setCurrentIndex(1);}

void MainWindow::on_teach_load_file_clicked() {
    /*
    QString path = load_file();
    if (!path.isEmpty()) {
        load_teach(path);
    }*/
    loadFileAndSwitchToProperTab();
}



bool MainWindow::load_teach(QString path, bool multi_file) {
    if (!checkFile(TEACH, path)) {
        // enable gui elements
        ui->teach_sort->setEnabled(true);
        ui->teach_delete_sort->setEnabled(true);
        ui->teach_new_sort->setEnabled(true);
        ui->teach_filter_from->setEnabled(true);
        ui->teach_filter_to->setEnabled(true);
        ui->teach_pie_button->setEnabled(true);
        ui->teach_bar_button->setEnabled(true);
        ui->teach_stat_button->setEnabled(true);
        ui->teach_to_label->setEnabled(true);
        ui->teach_sort_label->setEnabled(true);
        ui->teach_filter->setEnabled(true);
        ui->teach_filter_label->setEnabled(true);

        // load save order
        QSortListIO teachSaveOrder(TEACHORDER_SAVE);
        allTeachOrders = teachSaveOrder.readList();
        ui->teach_sort->clear();
        for (int i = 0; i < allTeachOrders.size(); i++) {
            ui->teach_sort->addItem(allTeachOrders.at(i).at(0));
        }

        // create default sort order if none are loaded
        if (ui->teach_sort->currentIndex() < 0) {
            createDefaultSortOrder(TEACH);
            ui->teach_sort->addItem(allTeachOrders[0][0]);
        }

        // create the tree
        teachPath = path;
        makeTree(TEACH);
        ui->teach_file_label->setText(teachPath);

        // Serialize loaded Model - Team Draco 2016
        serialize_loaded_paths(PREVIOUS_FILES_SAVE);

        return true;
    } else {
        if (!multi_file) {
            QMessageBox::critical(this, "Invalid File", "Not a valid teaching file.");
            on_teach_load_file_clicked();
        }
    }
    return false;
}

void MainWindow::on_pub_load_file_clicked() {
    /*
    QString path = load_file();
    if (!path.isEmpty()) {
        load_pub(path);
    }*/
    loadFileAndSwitchToProperTab();
}

bool MainWindow::load_pub(QString path, bool multi_file) {
    if (!checkFile(PUBLICATIONS, path)) {
        // enable gui elements
        ui->pub_sort->setEnabled(true);
        ui->pub_delete_sort->setEnabled(true);
        ui->pub_new_sort->setEnabled(true);
        ui->pub_filter_from->setEnabled(true);
        ui->pub_filter_to->setEnabled(true);
        ui->pub_pie_button->setEnabled(true);
        ui->pub_bar_button->setEnabled(true);
        ui->pub_stat_button->setEnabled(true);
        ui->pub_to_label->setEnabled(true);
        ui->pub_sort_label->setEnabled(true);
        ui->pub_filter->setEnabled(true);
        ui->pub_filter_label->setEnabled(true);

        // load save order
        QSortListIO pubSaveOrder(PUBORDER_SAVE);
        allPubOrders = pubSaveOrder.readList();
        ui->pub_sort->clear();
        for (int i = 0; i < allPubOrders.size(); i++) {
            ui->pub_sort->addItem(allPubOrders.at(i).at(0));
        }

        // create default sort order if none are loaded
        if (ui->pub_sort->currentIndex() < 0) {
            createDefaultSortOrder(PUBLICATIONS);
            ui->pub_sort->addItem(allPubOrders[0][0]);
        }

        // create the tree
        pubPath = path;
        makeTree(PUBLICATIONS);
        ui->pub_file_label->setText(pubPath);

        // Serialize loaded Model - Team Draco 2016
        serialize_loaded_paths(PREVIOUS_FILES_SAVE);

        return true;
    } else {
        if (!multi_file) {
            QMessageBox::critical(this, "Invalid File", "Not a valid publications file.");
            on_pub_load_file_clicked();
        }
    }
    return false;
}

void MainWindow::on_pres_load_file_clicked() {
    /*
    QString path = load_file();
    if (!path.isEmpty()) {
        load_pres(path);
    }*/
    loadFileAndSwitchToProperTab();
}

bool MainWindow::load_pres(QString path, bool multi_file) {
    if (!checkFile(PRESENTATIONS, path)) {
        // enable gui elements
        ui->pres_sort->setEnabled(true);
        ui->pres_delete_sort->setEnabled(true);
        ui->pres_new_sort->setEnabled(true);
        ui->pres_filter_from->setEnabled(true);
        ui->pres_filter_to->setEnabled(true);
        ui->pres_pie_button->setEnabled(true);
        ui->pres_bar_button->setEnabled(true);
        ui->pres_stat_button->setEnabled(true);
        ui->pres_to_label->setEnabled(true);
        ui->pres_sort_label->setEnabled(true);
        ui->pres_filter->setEnabled(true);
        ui->pres_filter_label->setEnabled(true);

        // load save order
        QSortListIO presSaveOrder(PRESORDER_SAVE);
        allPresOrders = presSaveOrder.readList();
        ui->pres_sort->clear();
        for (int i = 0; i < allPresOrders.size(); i++) {
            ui->pres_sort->addItem(allPresOrders.at(i).at(0));
        }

        // create default sort order if none are loaded
        if (ui->pres_sort->currentIndex() < 0) {
            createDefaultSortOrder(PRESENTATIONS);
            ui->pres_sort->addItem(allPresOrders[0][0]);
        }

        // create the tree
        presPath = path;
        makeTree(PRESENTATIONS);
        ui->pres_file_label->setText(presPath);

        // Serialize loaded Model - Team Draco 2016
        serialize_loaded_paths(PREVIOUS_FILES_SAVE);

        return true;
    } else {
        if (!multi_file) {
            QMessageBox::critical(this, "Invalid File", "Not a valid presentations file.");
            on_pres_load_file_clicked();
        }
    }
    return false;
}

void MainWindow::on_fund_load_file_clicked() {
    /*QString path = load_file();
    if (!path.isEmpty()) {
        load_fund(path);
    }*/
    loadFileAndSwitchToProperTab();
}

bool MainWindow::load_fund(QString path, bool multi_file) {
    if (!checkFile(FUNDING, path)) {
        // enable gui elements
        ui->fund_sort->setEnabled(true);
        ui->fund_delete_sort->setEnabled(true);
        ui->fund_new_sort->setEnabled(true);
        ui->fund_filter_from->setEnabled(true);
        ui->fund_filter_to->setEnabled(true);
        ui->fund_pie_button->setEnabled(true);
        ui->fund_bar_button->setEnabled(true);
        ui->fund_stat_button->setEnabled(true);
        ui->fund_to_label->setEnabled(true);
        ui->fund_sort_label->setEnabled(true);
        ui->fund_filter->setEnabled(true);
        ui->fund_filter_label->setEnabled(true);

        // load save order
        QSortListIO fundSaveOrder(FUNDORDER_SAVE);
        allFundOrders = fundSaveOrder.readList();
        ui->fund_sort->clear();
        for (int i = 0; i < allFundOrders.size(); i++) {
            ui->fund_sort->addItem(allFundOrders.at(i).at(0));
        }

        // create default sort order if none are loaded
        if (ui->fund_sort->currentIndex() < 0) {
            createDefaultSortOrder(FUNDING);
            ui->fund_sort->addItem(allFundOrders[0][0]);
        }

        // create the tree
        fundPath = path;
        makeTree(FUNDING);
        ui->fund_file_label->setText(fundPath);

        // Serialize loaded Model - Team Draco 2016
        serialize_loaded_paths(PREVIOUS_FILES_SAVE);

        return true;
    } else {
        if (!multi_file) {
            QMessageBox::critical(this, "Invalid File", "Not a valid grants and funding file.");
            on_fund_load_file_clicked();
        }
    }
    return false;
}

/**
 * @brief MainWindow::load_serialized_paths
 * @param path
 * @return
 */
bool MainWindow::load_serialized_paths(QString path) {

    // open the file for reading
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "no previous files data file loaded";
        return false;
    }

    // read the data serialized from the file
    QDataStream in(&file);

    // extract sort fields
    QStringList paths;
    in >> paths;

    // close the file, we're done
    file.close();

    //Load the Paths in and load them respectively based on tab
    //Known index positions
    if(paths.size() == 4) {
        qDebug() << "Read: " << paths[0] << " " << paths[1] << " " << paths[2] << " " << paths[3];


        bool reload = false;
        if(paths[0].size() > 0 || paths[1].size() > 0 || paths[2].size() > 0 || paths[3].size() > 0){
            reload = true;
        }

        if(reload){
            //Move Following Functionality Later
            //Ask user if they want to reload previous files
            QMessageBox::StandardButton userReply;
            userReply = QMessageBox::question(this, "Test", "Load Previous Files?", QMessageBox::Yes|QMessageBox::No);
            if (userReply == QMessageBox::Yes) {
                qDebug() << "Loading Previous Files";

                if(paths[0].size() > 0){
                    load_teach(paths[0]);
                }

                if(paths[1].size() > 0){
                    load_pub(paths[1]);
                }

                if(paths[2].size() > 0){
                    load_pres(paths[2]);
                }

                if(paths[3].size() > 0){
                    load_fund(paths[3]);
                }


            } else {
                qDebug() << "Not Loading Previous Files Clearing Data file";
                //Do Nothing Clear Previous Paths Data
                //serialize_loaded_paths(PEVIOUS_FILES_SAVE);
            }
        }

    } else {
        qDebug() << "Malformed Saved Paths File";
        return false;
    }

    return true;
}

/**
 * @brief MainWindow::serialize_loaded_paths
 * @param path
 * @return
 */
bool MainWindow::serialize_loaded_paths(QString path) {
    //PREVIOUS_FILES_SAVE
    qDebug() << "Saving to: " << path;
    qDebug() << this->teachPath << " " << this->pubPath << " " << this->presPath << " " << this->fundPath;

    QStringList paths;
    //QVector<String> paths;
    paths.append(this->teachPath);
    paths.append(this->pubPath);
    paths.append(this->presPath);
    paths.append(this->fundPath);

    // open the file for writing
    QFile file(path);
    file.open(QIODevice::WriteOnly);

    // we will serialize the data into the file
    QDataStream out(&file);

    // serialize the sort fields
    out << paths;

    // close the file, we're done
    file.close();

    return true;
}

void MainWindow::on_FromDate_dateChanged(const QDate &date) {
    // set the member variable to the new date
    yearStart = date.year();

    // update end date spinbox to not fall below that year
    ui->ToDate->setMinimumDate(date);

    dateChanged = {true, true, true, true};

    // refresh the GUI
    refresh(ui->categoryTab->currentIndex());
}

void MainWindow::on_ToDate_dateChanged(const QDate &date) {
    // set the member variable to the new date
    yearEnd = date.year();

    // update end date spinbox to not fall below that year
    ui->ToDate->setMinimumDate(ui->FromDate->date());

    dateChanged = {true, true, true, true};

    // refresh the GUI
    refresh(ui->categoryTab->currentIndex());
}

void MainWindow::on_categoryTab_currentChanged() {
    if (dateChanged[ui->categoryTab->currentIndex()] == true) {
        refresh(ui->categoryTab->currentIndex());
        dateChanged[ui->categoryTab->currentIndex()] = false;
    }
}

/**
 * @brief MainWindow::on_teachTreeView_clicked
 * @class MainWindow
 * @param index The Index of the Tree Clicked on
 * @details
 *  The
 */
void MainWindow::on_teachTreeView_clicked(const QModelIndex &index) {
    QString clickedName = index.data(Qt::DisplayRole).toString();
    if (clickedName==teachClickedName || index.column()!=0) { return;}

    std::vector<std::string> parentsList;
    QModelIndex current = index;
    QString name;
    while (true) {
        name = current.data(Qt::DisplayRole).toString();
        if(name!="") {
            auto it = parentsList.begin();
            it = parentsList.insert(it, name.toStdString());
        } else {
            break;
        }
        current = current.parent();
    }

    if (parentsList.size()!=teachSortOrder.size()) {
        teachClickedName = clickedName;
        std::vector<std::string> sortOrder(teachSortOrder.begin(), teachSortOrder.begin()+parentsList.size()+1);
        std::vector<std::pair <std::string, int>> list =
                teachdb->getCountTuple(yearStart, yearEnd, sortOrder, parentsList, getFilterStartChar(TEACH), getFilterEndChar(TEACH));
        std::vector<std::pair <std::string, double>> chartList;
        for (int i = 0; i < (int) list.size(); i++) {
            chartList.emplace_back(list[i].first, static_cast<double>(list[i].second));
        }

        if (!chartList.empty()) {
            ui->teachBarChart->clearPlottables();
            setupBarChart(ui->teachBarChart, chartList);
            ui->teachBarChart->replot();

            setupPieChart(ui->teachPieChart, ui->teachPieList, chartList);
            setupBoxPlot(ui->teachBoxPlot, chartList);

            if (parentsList.size()>1) {
                ui->teachGraphTitle->setText("Total " + clickedName + " Teaching by " +
                                             QString::fromStdString(teachSortOrder[parentsList.size()]) + " for " + QString::fromStdString(parentsList[0]));
            } else {
                ui->teachGraphTitle->setText("Total Teaching by " + QString::fromStdString(parentsList[0]));
            }
            ui->teach_graph_stackedWidget->show();
        }
    } else {
        ui->teach_graph_stackedWidget->hide();
        ui->teachGraphTitle->clear();
        teachClickedName.clear();
    }
}

void MainWindow::on_pubTreeView_clicked(const QModelIndex &index) {
    QString clickedName = index.data(Qt::DisplayRole).toString();
    if (clickedName==pubClickedName || index.column()!=0) { return;}

    std::vector<std::string> parentsList;
    QModelIndex current = index;
    QString name;
    while (true) {
        name = current.data(Qt::DisplayRole).toString();
        if(name!="") {
            auto it = parentsList.begin();
            it = parentsList.insert(it, name.toStdString());
        } else {
            break;
        }
        current = current.parent();
    }

    if (parentsList.size()!=pubSortOrder.size()) {
        pubClickedName = clickedName;
        std::vector<std::string> sortOrder(pubSortOrder.begin(), pubSortOrder.begin()+parentsList.size()+1);
        std::vector<std::pair <std::string, int>> list =
                pubdb->getCountTuple(yearStart, yearEnd, sortOrder, parentsList, getFilterStartChar(PUBLICATIONS), getFilterEndChar(PUBLICATIONS));
        std::vector<std::pair <std::string, double>> chartList;
        for (int i = 0; i < (int) list.size(); i++) {
            chartList.emplace_back(list[i].first, static_cast<double>(list[i].second));
        }

        if (!chartList.empty()) {
            ui->pubBarChart->clearPlottables();
            setupBarChart(ui->pubBarChart, chartList);
            ui->pubBarChart->replot();

            setupPieChart(ui->pubPieChart, ui->pubPieList, chartList);
            setupBoxPlot(ui->pubBoxPlot, chartList);

            if (parentsList.size()>1) {
                ui->pubGraphTitle->setText("Total " + clickedName + " Publications by " +
                                           QString::fromStdString(pubSortOrder[parentsList.size()]) + " for " + QString::fromStdString(parentsList[0]));
            } else {
                ui->pubGraphTitle->setText("Total Publications by " + QString::fromStdString(parentsList[0]));
            }
            ui->pub_graph_stackedWidget->show();
        }
    } else {
        ui->pub_graph_stackedWidget->hide();
        ui->pubGraphTitle->clear();
        pubClickedName.clear();
    }
}

void MainWindow::on_presTreeView_clicked(const QModelIndex &index) {
    QString clickedName = index.data(Qt::DisplayRole).toString();
    if (clickedName==presClickedName || index.column()!=0) { return;}

    std::vector<std::string> parentsList;
    QModelIndex current = index;
    QString name;
    while (true) {
        name = current.data(Qt::DisplayRole).toString();
        if(name!="") {
            auto it = parentsList.begin();
            it = parentsList.insert(it, name.toStdString());
        } else {
            break;
        }
        current = current.parent();
    }

    if (parentsList.size()!=presSortOrder.size()) {
        presClickedName = clickedName;
        std::vector<std::string> sortOrder(presSortOrder.begin(), presSortOrder.begin()+parentsList.size()+1);
        std::vector<std::pair <std::string, int>> list =
                presdb->getCountTuple(yearStart, yearEnd, sortOrder, parentsList, getFilterStartChar(PRESENTATIONS), getFilterEndChar(PRESENTATIONS));
        std::vector<std::pair <std::string, double>> chartList;
        for (int i = 0; i < (int) list.size(); i++) {
            chartList.emplace_back(list[i].first, static_cast<double>(list[i].second));
        }

        if (!chartList.empty()) {
            ui->presBarChart->clearPlottables();
            setupBarChart(ui->presBarChart, chartList);
            ui->presBarChart->replot();

            setupPieChart(ui->presPieChart, ui->presPieList, chartList);
            setupBoxPlot(ui->presBoxPlot, chartList);

            if (parentsList.size()>1) {
                ui->presGraphTitle->setText("Total " + clickedName + " Presentations by " +
                                            QString::fromStdString(presSortOrder[parentsList.size()]) + " for " + QString::fromStdString(parentsList[0]));
            } else {
                ui->presGraphTitle->setText("Total Presentations by " + QString::fromStdString(parentsList[0]));
            }
            ui->pres_graph_stackedWidget->show();
        }
    } else {
        ui->pres_graph_stackedWidget->hide();
        ui->presGraphTitle->clear();
        presClickedName.clear();
    }
}

void MainWindow::on_fundTreeView_clicked(const QModelIndex &index) {
    QString clickedName = index.data(Qt::DisplayRole).toString();
    if (clickedName==fundClickedName || index.column()!=0) { return;}

    std::vector<std::string> parentsList;
    QModelIndex current = index;
    QString name;
    while (true) {
        name = current.data(Qt::DisplayRole).toString();
        if(name!="") {
            auto it = parentsList.begin();
            it = parentsList.insert(it, name.toStdString());
        } else {
            break;
        }
        current = current.parent();
    }

    if (parentsList.size()!=fundSortOrder.size()) {
        if (clickedName != fundClickedName) {
            fundClickedName = clickedName;
            std::vector<std::string> sortOrder(fundSortOrder.begin(), fundSortOrder.begin()+parentsList.size()+1);
            std::vector<std::pair <std::string, double>> chartList =
                    funddb->getTotalsTuple(yearStart, yearEnd, sortOrder, parentsList, "Total Amount", getFilterStartChar(FUNDING), getFilterEndChar(FUNDING));

            if (!chartList.empty()) {
                ui->fundBarChart->clearPlottables();
                setupBarChart(ui->fundBarChart, chartList);
                ui->fundBarChart->replot();

                setupPieChart(ui->fundPieChart, ui->fundPieList, chartList);
                setupBoxPlot(ui->fundBoxPlot, chartList);

                if (parentsList.size()>1) {
                    ui->fundGraphTitle->setText("Total " + clickedName + " Grants & Funding by " +
                                                QString::fromStdString(fundSortOrder[parentsList.size()]) + " for " + QString::fromStdString(parentsList[0]));
                } else {
                    ui->fundGraphTitle->setText("Total Grants & Funding by " + QString::fromStdString(parentsList[0]));
                }
                ui->fund_graph_stackedWidget->show();
            }
        } else {
            ui->fund_graph_stackedWidget->hide();
            ui->fundGraphTitle->clear();
            fundClickedName.clear();
        }
    }
}

void MainWindow::on_teachPrintButton_clicked()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QCPPainter painter;
        painter.begin(printer);
        ui->teachChartFrame->render(&painter);
    }
}

void MainWindow::on_fundPrintButton_clicked()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QCPPainter painter;
        painter.begin(printer);
        ui->fundChartFrame->render(&painter);
    }
}

void MainWindow::on_presPrintButton_clicked()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QCPPainter painter;
        painter.begin(printer);
        ui->presChartFrame->render(&painter);
    }
}

void MainWindow::on_pubPrintButton_clicked()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QCPPainter painter;
        painter.begin(printer);
        ui->pubChartFrame->render(&painter);
    }
}

void MainWindow::on_teachExportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"),
                                                    QDir::homePath(),
                                                    tr("PDF (*.pdf)"));
    if (fileName.contains("") != 0) {
        QPdfWriter writer(fileName);
        writer.setPageOrientation(QPageLayout::Landscape);
        writer.setPageSize(QPagedPaintDevice::A4);
        writer.setPageMargins(QMargins(40, 40, 40, 40));

        QPainter painter(&writer);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Times", 20));

        QDate date = QDate::currentDate();
//        QString currentdate =  date.toString("dd.MM.yyyy");
        QString currentdate =  date.toString("yyyy.MM.dd");
        currentdate = "Exported on " + currentdate;
        QRect r = painter.viewport();
        painter.drawText(r, Qt::AlignBottom, currentdate);
        painter.scale(15.0, 15.0);
        QSize preSize = ui->teachChartFrame->size();
        ui->teachChartFrame->setFixedHeight(515);
        ui->teachChartFrame->setFixedWidth(762);
        ui->teachChartFrame->render(&painter);

        //setting back the size
        ui->teachChartFrame->setFixedHeight(preSize.height());
         ui->teachChartFrame->setFixedHeight(preSize.width());
        /* Another option for bar chart since it is QCustom plot
        if (ui->teach_bar_button->isChecked()) {
            ui->teachBarChart->savePdf(fileName);
        } */
    }

}

void MainWindow::on_fundExportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"),
                                                    QDir::homePath(),
                                                    tr("PDF (*.pdf)"));
    if (fileName.contains("") != 0) {
        QPdfWriter writer(fileName);
        writer.setPageOrientation(QPageLayout::Landscape);
        writer.setPageSize(QPagedPaintDevice::A4);
        writer.setPageMargins(QMargins(40, 40, 40, 40));

        QPainter painter(&writer);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Times", 20));

        QDate date = QDate::currentDate();
//        QString currentdate =  date.toString("dd.MM.yyyy");
        QString currentdate =  date.toString("yyyy.MM.dd");
        currentdate = "Exported on " + currentdate;
        QRect r = painter.viewport();
        painter.drawText(r, Qt::AlignBottom, currentdate);
        painter.scale(15.0, 15.0);
        QSize preSize = ui->teachChartFrame->size();
        ui->fundChartFrame->setFixedHeight(515);
        ui->fundChartFrame->setFixedWidth(762);
        ui->fundChartFrame->render(&painter);

        //setting back the size
        ui->fundChartFrame->setFixedHeight(preSize.height());
         ui->fundChartFrame->setFixedHeight(preSize.width());
    }
}

void MainWindow::on_presExportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"),
                                                    QDir::homePath(),
                                                    tr("PDF (*.pdf)"));
    if (fileName.contains("") != 0) {
        QPdfWriter writer(fileName);
        writer.setPageOrientation(QPageLayout::Landscape);
        writer.setPageSize(QPagedPaintDevice::A4);
        writer.setPageMargins(QMargins(40, 40, 40, 40));

        QPainter painter(&writer);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Times", 20));

        QDate date = QDate::currentDate();
//        QString currentdate =  date.toString("dd.MM.yyyy");
        QString currentdate =  date.toString("yyyy.MM.dd");
        currentdate = "Exported on " + currentdate;
        QRect r = painter.viewport();
        painter.drawText(r, Qt::AlignBottom, currentdate);
        painter.scale(15.0, 15.0);
        QSize preSize = ui->teachChartFrame->size();
        ui->presChartFrame->setFixedHeight(515);
        ui->presChartFrame->setFixedWidth(762);
        ui->presChartFrame->render(&painter);

        //setting back the size
        ui->presChartFrame->setFixedHeight(preSize.height());
         ui->presChartFrame->setFixedHeight(preSize.width());
    }
}

void MainWindow::on_pubExportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export File"),
                                                    QDir::homePath(),
                                                    tr("PDF (*.pdf)"));
    if (fileName.contains("") != 0) {
        QPdfWriter writer(fileName);
        writer.setPageOrientation(QPageLayout::Landscape);
        writer.setPageSize(QPagedPaintDevice::A4);
        writer.setPageMargins(QMargins(40, 40, 40, 40));

        QPainter painter(&writer);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Times", 20));

        QDate date = QDate::currentDate();
//        QString currentdate =  date.toString("dd.MM.yyyy");
        QString currentdate =  date.toString("yyyy.MM.dd");
        currentdate = "Exported on " + currentdate;
        QRect r = painter.viewport();
        painter.drawText(r, Qt::AlignBottom, currentdate);
        painter.scale(15.0, 15.0);
        QSize preSize = ui->teachChartFrame->size();
        ui->pubChartFrame->setFixedHeight(515);
        ui->pubChartFrame->setFixedWidth(762);
        ui->pubChartFrame->render(&painter);

        //setting back the size
        ui->pubChartFrame->setFixedHeight(preSize.height());
         ui->pubChartFrame->setFixedHeight(preSize.width());
    }
}


char MainWindow::getFilterStartChar(int type) {
    char charInField;

    // get char from the text field
    switch (type) {
    case FUNDING:
        charInField = ui->fund_filter_from->text().toStdString()[0];
        break;
    case PRESENTATIONS:
        charInField = ui->pres_filter_from->text().toStdString()[0];
        break;
    case PUBLICATIONS:
        charInField = ui->pub_filter_from->text().toStdString()[0];
        break;
    case TEACH:
        charInField = ui->teach_filter_from->text().toStdString()[0];
        break;
    }

    // convert the char to uppercase
    char uppercase = charInField & ~0x20;

    // if alphabetical, return that letter
    if ('A' <= uppercase && uppercase <= 'Z') {
        return uppercase;
    } else {
        // otherwise, default is '*'
        return '*';
    }
}

char MainWindow::getFilterEndChar(int type) {
    char charInField;

    // get char from the text field
    switch (type) {
    case FUNDING:
        charInField = ui->fund_filter_to->text().toStdString()[0];
        break;
    case PRESENTATIONS:
        charInField = ui->pres_filter_to->text().toStdString()[0];
        break;
    case PUBLICATIONS:
        charInField = ui->pub_filter_to->text().toStdString()[0];
        break;
    case TEACH:
        charInField = ui->teach_filter_to->text().toStdString()[0];
        break;
    }

    // convert the char to uppercase
    char uppercase = charInField & ~0x20;

    // if alphabetical, return that letter
    if ('A' <= uppercase && uppercase <= 'Z') {
        return uppercase;
    } else if (charInField == '*') {
        // otherwise, is it '*'?
        return charInField;
    } else {
        // otherwise, default is 'Z'
        return 'Z';
    }
}

void MainWindow::on_teach_filter_from_textChanged() { refresh(TEACH);}
void MainWindow::on_teach_filter_to_textChanged() { refresh(TEACH);}
void MainWindow::on_pub_filter_from_textChanged() { refresh(PUBLICATIONS);}
void MainWindow::on_pub_filter_to_textChanged() { refresh(PUBLICATIONS);}
void MainWindow::on_pres_filter_from_textChanged() { refresh(PRESENTATIONS);}
void MainWindow::on_pres_filter_to_textChanged() { refresh(PRESENTATIONS);}
void MainWindow::on_fund_filter_from_textChanged() { refresh(FUNDING);}
void MainWindow::on_fund_filter_to_textChanged() { refresh(FUNDING);}

//Adding QTestClasses as Friends
//friend Rectangle duplicate (const Rectangle&);


