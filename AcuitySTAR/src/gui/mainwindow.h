#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include <QApplication>
#include <QTreeView>
#include <QMainWindow>
#include <./testing/test.h>


class RecordsManager;
class CSVReader;
class TreeModel;
class CustomSort;
class PieChartWidget;
class QListWidget;
class QTreeView;
class QCustomPlot;
class QPrinter;
class QPainter;
class QTTestLoadFile;
class QTabWidget;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private slots:
    QString load_file();

    void on_pres_load_file_clicked();
    void on_pub_load_file_clicked();
    void on_fund_load_file_clicked();
    void on_teach_load_file_clicked();

    void on_teach_new_sort_clicked();
    void on_pub_new_sort_clicked();
    void on_pres_new_sort_clicked();
    void on_fund_new_sort_clicked();

    void on_actionLoad_file_triggered();

    void on_FromDate_dateChanged(const QDate &date);
    void on_ToDate_dateChanged(const QDate &date);

    void refresh(int tabIndex);

    void on_teach_sort_currentIndexChanged(int index);
    void on_pub_sort_currentIndexChanged(int index);
    void on_pres_sort_currentIndexChanged(int index);
    void on_fund_sort_currentIndexChanged(int index);

    void on_teach_delete_sort_clicked();
    void on_pub_delete_sort_clicked();
    void on_pres_delete_sort_clicked();
    void on_fund_delete_sort_clicked();

    void on_teach_pie_button_toggled();
    void on_teach_bar_button_toggled();
    void on_teach_stat_button_toggled();
    void on_teach_line_button_toggled();
    void on_pub_pie_button_toggled();
    void on_pub_bar_button_toggled();
    void on_pub_stat_button_toggled();
    void on_pub_line_button_toggled();
    void on_pres_pie_button_toggled();
    void on_pres_bar_button_toggled();
    void on_pres_stat_button_toggled();
    void on_pres_line_button_toggled();
    void on_fund_pie_button_toggled();
    void on_fund_bar_button_toggled();
    void on_fund_stat_button_toggled();
    void on_fund_line_button_toggled();

    void on_teachTreeView_clicked(const QModelIndex &index);
    void on_pubTreeView_clicked(const QModelIndex &index);
    void on_presTreeView_clicked(const QModelIndex &index);
    void on_fundTreeView_clicked(const QModelIndex &index);

    void on_categoryTab_currentChanged();

    void on_teach_filter_from_textChanged();
    void on_teach_filter_to_textChanged();
    void on_pub_filter_from_textChanged();
    void on_pub_filter_to_textChanged();
    void on_pres_filter_from_textChanged();
    void on_pres_filter_to_textChanged();
    void on_fund_filter_from_textChanged();
    void on_fund_filter_to_textChanged();

    void on_teachPrintButton_clicked();

    void on_fundPrintButton_clicked();

    void on_presPrintButton_clicked();

    void on_pubPrintButton_clicked();

    void on_teachExportButton_clicked();

    void on_fundExportButton_clicked();

    void on_presExportButton_clicked();

    void on_pubExportButton_clicked();

    //void on_getStartedButton_clicked();

    //void on_stackedWidget_destroyed();

//    void on_teach_stat_button_clicked();

//    void on_teach_stat_button_clicked(bool checked);

//    void on_teach_stat_button_toggled(bool checked);

    //void on_teachTreeView_pressed(const QModelIndex &index);

private:
    static std::vector<std::string> GRANTS_MANFIELDS, PRES_MANFIELDS, PUBS_MANFIELDS, TEACH_MANFIELDS;

    enum TABS {
        TEACH, PUBLICATIONS, PRESENTATIONS, FUNDING
    };

    //Selection ofFlat UI Colors
    QColor flatColors[8] = {
                QColor(44, 62, 80),
                QColor(142, 68, 173),
                QColor(41, 128, 185),
                QColor(39, 174, 96),
                QColor(22, 160, 133),
                QColor(192, 57, 43),
                QColor(211, 84, 0),
                QColor(243, 156, 18)
            };

    struct field_error;

    Ui::MainWindow* ui;
    QPrinter* printer;

    QList<QStringList> allTeachOrders, allPubOrders, allPresOrders, allFundOrders;
    QString teachPath, pubPath, presPath, fundPath;
    TreeModel *fundTree, *presTree, *pubTree, *teachTree;
    RecordsManager *funddb, *presdb, *pubdb, *teachdb;
    std::vector<std::vector<std::string>> fundData, presData, pubData, teachData;

    std::vector<std::string> teachSortOrder, pubSortOrder, presSortOrder, fundSortOrder;
    TreeModel* currentTree;
    RecordsManager* currentdb;
    QTreeView* currentView;
    QString teachClickedName, pubClickedName, presClickedName, fundClickedName;

    QList<bool> dateChanged;

    std::vector<std::string> getParentsList(QModelIndex &index);
    char getFilterStartChar(int type);
    char getFilterEndChar(int type);

    int yearStart, yearEnd;

    int checkFile(int index, QString filePath);
    //std::vector<std::string> getSelectedSortOrder(int tabIndex);
    void createDefaultSortOrder(int tabIndex);
    void makeTree(int tabIndex);

    void setupPieChart(PieChartWidget *pieChart, QListWidget *pieListWidget, std::vector<std::pair<std::string, double> > pieChartList);
    void setupBarChart(QCustomPlot *barChart, std::vector<std::pair<std::string, double> > barChartList);
    void setupBoxPlot(QCustomPlot *boxPlot, std::vector<std::pair <std::string, double>> boxPlotList);
    void setupLineChart(QCustomPlot *lineChart, std::vector<std::pair <std::string, double>> lineChartList);

    void median(std::vector<double> &data, int low, int high, double &median, int &newLow, int &newHigh);

    bool handle_field_errors(std::vector<std::vector<std::string>*>& errors,
                             std::vector<std::string>& headers,
                             std::vector<std::string>& mandatory,
                             QString filePath,
                             TABS tab);
    bool load_fund(QString path, bool multi_file = false);
    bool load_pub(QString path, bool multi_file = false);
    bool load_teach(QString path, bool multi_file = false);
    bool load_pres(QString path, bool multi_file = false);

    bool serialize_loaded_paths(QString save_path);
    bool load_serialized_paths(QString save_path);
    friend class QTTestLoadFile;

    bool checkTargetTabHasFileLoaded(TABS);
    void loadFileAndSwitchToProperTab();
};

#endif // MAINWINDOW_H
