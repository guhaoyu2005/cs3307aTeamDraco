#include "ui_ErrorEditDialog.h"
#include "ErrorEditDialog.h"
#include "database/CSVWriter.h"

#include <QTableWidgetItem>
#include <QStringList>
#include <QDebug>
#include <QBrush>
#include <QColor>
#include <QAbstractButton>
#include <QMessageBox>
#include <QItemSelectionModel>

bool ErrorEditDialog::isManfields(CSVReader::CSVFileType fileType, std::string cmpString) {
    switch (fileType) {
    case CSVReader::CSVFileTypeTeaching: {
        for (int i=0;i<TEACH_MANFIELDS.size();i++)
            if (TEACH_MANFIELDS[i]==cmpString)
                return true;
        break;
    }
    case CSVReader::CSVFileTypePublications: {
        for (int i=0;i<PUBS_MANFIELDS.size();i++)
            if (PUBS_MANFIELDS[i]==cmpString)
                return true;
        break;
    }
    case CSVReader::CSVFileTypePresentations: {
        for (int i=0;i<PRES_MANFIELDS.size();i++)
            if (PRES_MANFIELDS[i]==cmpString)
                return true;
        break;
    }
    case CSVReader::CSVFileTypeGrants: {
        for (int i=0;i<GRANTS_MANFIELDS.size();i++)
            if (GRANTS_MANFIELDS[i]==cmpString)
                return true;
        break;
    }
    }
    return false;
}

/*
 * Load data contained in the errors vector into a QWidgetTable
 * Fields will be marked red and editable if they are a mandatory field
 * and editable.  Otherwise all other fields will not be editable.
 * Clicking Save in the dialog will return the corrected entries to the main
 * program through the errors parameter.  If not all marked fields are edited
 * then a warning message will be displayed.  If cancel is clicked all errors
 * are discarded.
 */
ErrorEditDialog::ErrorEditDialog(QWidget *parent,
                CSVReader::CSVFileType fileType,
                std::string filePath) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    type(fileType),
    path(filePath),
    ui(new Ui::ErrorEditDialog)
{   
    ui->setupUi(this);
    ErrorCounter = 0;
    FixedErrorCounter = 0;

    QStringList listHeaders;

    CSVReader reader = CSVReader(filePath);
    reader.loadCSV(filePath);
    std::string errMsg;
    header = reader.getHeaders();
    for (int i=0; i<header.size();i++) {
        listHeaders<<header[i].c_str();
    }
    data.clear();
    data = reader.getData();
    ErrorLoc.clear();

    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableWidget->setRowCount((int) data.size());
    ui->tableWidget->setColumnCount((int) header.size());

    ui->tableWidget->setHorizontalHeaderLabels(listHeaders);
    QTableWidgetItem* item;
    QBrush brushRed(QColor(255, 0, 0, 100));
    QBrush brushWhite(QColor(255, 255, 255, 100));
    std::vector<std::vector<std::string>>::iterator it;
    int row = 0;
    for (it = data.begin(); it != data.end(); it++) {
        for (int col = 0; col < (int) header.size(); col++) {
            item = new QTableWidgetItem();
            if (col < (int) (it)->size())
                item->setText((it)->at(col).c_str());
            else
                item->setText("");

            if (isManfields(type, header[col])) {
                if ((it)->at(col).compare("") == 0) {
                    ErrorCounter++;
                    item->setBackground(brushRed);
                    ErrorLoc.push_back(row*(int)header.size()+col);
                }
            }
            ui->tableWidget->setItem(row, col, item);
        }
        row++;
    }

    ui->ErrorDisplayLbl->setText("Error Fixed: "+ QString::number(FixedErrorCounter) +" Remaining: " + QString::number(ErrorCounter-FixedErrorCounter));
}

//Clean up allocated memory for the table items
ErrorEditDialog::~ErrorEditDialog()
{
    /*
    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        for (int j = 0; j < ui->tableWidget->columnCount(); j++) {
            delete ui->tableWidget->item(i,j);
        }
    }
    delete ui;
    */
    ui->tableWidget->clearContents();
    ui->tableWidget->clear();
    delete ui;
}

bool ErrorEditDialog::errorCheck() {
    return true;
    QBrush brushRed(QColor(255, 0, 0, 100));
    QBrush brushNormal(QColor(0, 0, 0, 100));
    bool rt = true;
    //true for passing
    for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
        for (int col = 0; col < ui->tableWidget->columnCount(); col++) {
            if (data[row][col].compare("") != 0) {
                ui->tableWidget->item(row, col)->setBackground(brushRed);
                rt = false;
            }
            else {
                ui->tableWidget->item(row, col)->setBackground(brushNormal);
            }
        }
    }
    return rt;
}

void ErrorEditDialog::saveToFile() {

}

//Save the new data entered by the user via the error reference var
void ErrorEditDialog::saveData() {
    if (!errorCheck()) {
        QMessageBox::StandardButton msgbox;
        msgbox = QMessageBox::question(this, "Info", "There are some errors exist still, continue? \n", QMessageBox::Yes|QMessageBox::No);
        if (msgbox == QMessageBox::No) {
            //do nothing and return
            return;
        }
    }

    CSVWriter writer;
    if (writer.isExistingBackup(path)) {
        QMessageBox::StandardButton msgbox;
        msgbox = QMessageBox::question(this, "Info", "Seems your last saving process was failed, continue? \n", QMessageBox::Yes|QMessageBox::No);
        if (msgbox == QMessageBox::No) {
            //do nothing and return
            return;
        }
    }

    data.clear();
    for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
        std::vector<std::string> pVec;
        for (int col = 0; col < ui->tableWidget->columnCount(); col++) {
            pVec.push_back((ui->tableWidget->item(row, col)->text()=="")?"":ui->tableWidget->item(row, col)->text().toStdString());
            //qDebug ("ROW:%d COL:%d-> %s\n",row,col,ui->tableWidget->item(row, col)->text().toStdString().c_str());
        }
        data.push_back(pVec);
    }

    if (writer.writeToCSV(path, header, data)) {
        //success
    }
    else {
        QMessageBox::StandardButton msgbox;
        msgbox = QMessageBox::information(this, "Info", "Failed to save your data.\n", QMessageBox::Ok);
        if (msgbox == QMessageBox::Ok) {
            //do nothing and return
            return;
        }
        return;
    }

/*
    if (type==CSVReader::CSVFileTypeGrants) {
        if (writer.writeToCSVDueToShitCodeForGrantShits(path, header, data)) {
            //success
        }
        else {
            QMessageBox::StandardButton msgbox;
            msgbox = QMessageBox::information(this, "Info", "Failed to save your data.\n", QMessageBox::Ok);
            if (msgbox == QMessageBox::Ok) {
                //do nothing and return
                return;
            }
            return;
        }
    }
    else {
        if (writer.writeToCSV(path, header, data)) {
            //success
        }
        else {
            QMessageBox::StandardButton msgbox;
            msgbox = QMessageBox::information(this, "Info", "Failed to save your data.\n", QMessageBox::Ok);
            if (msgbox == QMessageBox::Ok) {
                //do nothing and return
                return;
            }
            return;
        }
    }
*/



    /*
    for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
        for (int col = 0; col < ui->tableWidget->columnCount() && col < (int) errorList[row]->size(); col++) {
            std::vector<std::string>::iterator it = errorList[row]->begin()+col;
            if (errorList[row]->at(col).compare("") == 0) {
                it = errorList[row]->erase(it);
                errorList[row]->insert(it, ui->tableWidget->item(row, col)->text().toStdString());
            }
        }
    }
    */
    accept();
}

void ErrorEditDialog::on_save_clicked()
{
    bool search = true;

    for (int row = 0; row < ui->tableWidget->rowCount() && search; row++) {
        for (int j = 0; j < (int) mandatoryList.size() && search; j++) {
            std::vector<std::string>::iterator it = std::find(headerList.begin(), headerList.end(), mandatoryList[j]);
            int col = it - headerList.begin();
            QTableWidgetItem* item = ui->tableWidget->item(row, col);
            data[row][col] = item->text().toStdString();
        }
    }

    //check if mandatory fields have been filled
    for (int row = 0; row < ui->tableWidget->rowCount() && search; row++) {
        for (int j = 0; j < (int) mandatoryList.size() && search; j++) {
            std::vector<std::string>::iterator it = std::find(headerList.begin(), headerList.end(), mandatoryList[j]);
            int col = it - headerList.begin();
            QTableWidgetItem* item = ui->tableWidget->item(row, col);
            if (item->text().compare("") == 0) {
                QMessageBox::critical(this, "Error", "Mandatory fields are still empty.");
                search = false;
            }
        }
    }
    if (search) {
        saveData();
    }
}

void ErrorEditDialog::on_cancel_clicked()
{
    reject();
}

void ErrorEditDialog::on_FindNextBtn_clicked() {
    if (ui->tableWidget->selectedItems().count()>0) {
        int currLoc = ui->tableWidget->selectedItems()[0]->row()*header.size()+ui->tableWidget->selectedItems()[0]->column();
        int nextError = -1;
        for (int i=0;i<ErrorLoc.size();i++) {
            if (ErrorLoc[i]>currLoc) {
                nextError = ErrorLoc[i];
                break;
            }
        }
        for (int i=0;i<ui->tableWidget->selectedItems().count();i++) {
            ui->tableWidget->selectedItems()[i]->setSelected(false);
        }
        if (nextError == -1) {
            //select the first one
            ui->tableWidget->setCurrentCell(ErrorLoc[0]/header.size(), ErrorLoc[0]%header.size());
            //ui->tableWidget->itemAt(ErrorLoc[0]/header.size(), ErrorLoc[0]%header.size())->setSelected(true);
            //ui->tableWidget->scrollToItem(ui->tableWidget->itemAt(ErrorLoc[0]/header.size(), ErrorLoc[0]%header.size()));
        }
        else {
            ui->tableWidget->setCurrentCell(nextError/header.size(), nextError%header.size());
            //ui->tableWidget->itemAt(nextError/header.size(), nextError%header.size())->setSelected(true);
            //ui->tableWidget->scrollToItem(ui->tableWidget->itemAt(nextError/header.size(), nextError%header.size()));
        }
    }
    else {
        QMessageBox::critical(this, "Error", "Please select a cell first!");
    }
}

void ErrorEditDialog::on_FindPrevBtn_clicked() {
    if (ui->tableWidget->selectedItems().count()>0) {
        int currLoc = ui->tableWidget->selectedItems()[0]->row()*header.size()+ui->tableWidget->selectedItems()[0]->column();
        int nextError = -1;
        for (int i=ErrorLoc.size()-1;i>=0;i--) {
            if (ErrorLoc[i]<currLoc) {
                nextError = ErrorLoc[i];
                break;
            }
        }
        for (int i=0;i<ui->tableWidget->selectedItems().count();i++) {
            ui->tableWidget->selectedItems()[i]->setSelected(false);
        }
        //printf("BACK: %d", nextError);
        if (nextError == -1) {
            //select the last one
            ui->tableWidget->setCurrentCell(ErrorLoc[ErrorLoc.size()-1]/header.size(), ErrorLoc[ErrorLoc.size()-1]%header.size());
        }
        else {
            ui->tableWidget->setCurrentCell(nextError/header.size(), nextError%header.size());
        }

    }
    else {
        QMessageBox::critical(this, "Error", "Please select a cell first!");
    }

}

void ErrorEditDialog::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    int curr = item->row()*header.size()+item->column();
    for (int i=ErrorLoc.size()-1;i>=0;i--) {
        if (ErrorLoc[i]==curr) {
            if (item->text().compare("") == 0) {

            }
            else {
                FixedErrorCounter++;
                ui->ErrorDisplayLbl->setText("Error Fixed: "+ QString::number(FixedErrorCounter) +" Remaining: " + QString::number(ErrorCounter-FixedErrorCounter));
            }
            break;
        }
    }
}
