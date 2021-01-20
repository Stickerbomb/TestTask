#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "xmlparser.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QtXml>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_open_file_clicked();

    void on_choose_file_button_2_clicked();


    void on_Save_to_json_Button_clicked();


    void on_addButton_clicked();

private:

    QMenu *fileMenu;
    QJsonDocument jsondocument;
    QDomDocument document;
    XmlParser *xmlParser;
    Ui::MainWindow *ui;
    QStandardItemModel *model;
};
#endif // MAINWINDOW_H
