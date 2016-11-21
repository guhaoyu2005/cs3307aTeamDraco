#ifndef ERROREDITDIALOG_H
#define ERROREDITDIALOG_H

#include <QDialog>
#include <vector>
#include <string>
#include "../database/CSVReader.h"

class QAbstractButton;

namespace Ui {
    class ErrorEditDialog;
}

class ErrorEditDialog : public QDialog
{
    Q_OBJECT

public:
     ErrorEditDialog(QWidget *parent,
                     CSVReader::CSVFileType fileType,
                     std::string filePath);

    ~ErrorEditDialog();

private slots:
     void on_save_clicked();
     void on_cancel_clicked();

private:
     std::vector<std::string> GRANTS_MANFIELDS = {"Member Name", "Funding Type", "Status", "Peer Reviewed?", "Role", "Title", "Start Date"};
     std::vector<std::string> PRES_MANFIELDS = {"Member Name", "Date", "Type", "Role", "Title"};
     std::vector<std::string> PUBS_MANFIELDS = {"Member Name", "Type", "Status Date", "Role", "Title"};
     std::vector<std::string> TEACH_MANFIELDS = {"Member Name", "Start Date", "Program"};

private:
    std::vector<std::vector<std::string>*> errorList;
    std::vector<std::string> headerList;
    std::vector<std::string> mandatoryList;
    std::string path;
    CSVReader::CSVFileType type;
    Ui::ErrorEditDialog *ui;

    std::vector<std::vector<std::string>> data;
    std::vector<std::string> header;

    bool errorCheck();
    void saveData();
    bool isManfields(CSVReader::CSVFileType, std::string);
    void saveToFile();
};

#endif // ERROREDITDIALOG_H
