#include "homepage.h"
#include "ui_homepage.h"
#include "mainwindow.h"


HomePage::HomePage(QWidget *parent) :
    QMainWindow(parent),

    ui(new Ui::HomePage)

{
    ui->setupUi(this);
    QPixmap logo(":/logo.png");
    ui->label->setPixmap(logo);
   // ui->label->setAlignment(Qt::AlignCenter);
}


HomePage::~HomePage()
{
    delete ui;
}

void HomePage::on_commandLinkButton_clicked()
{
    MainWindow *w = new MainWindow();
    w->show();
    this->close();
}
