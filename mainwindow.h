#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    void on_choose_file_button_clicked();

    void on_choose_file_button_2_clicked();

    void on_Load_from_json_Button_clicked();

    void on_Save_to_json_Button_clicked();


private:

    QMenu *fileMenu;
    QJsonDocument jsondocument;
    QDomDocument document;
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    void read(QString _noed);
    void writeXML(QStandardItem *item, QDomNode &dom_root);
    void writeJson(QStandardItem *item, QJsonObject &json_root);
    void writeJson(QStandardItem *item, QJsonArray &json_root);
    QJsonValue* stringToJson(QString str);
    void traverseShow(const QDomNode &_elem, QStandardItem *subModel);
    QStandardItem* toStdItem(const QJsonArray &jarray, QString parent);
    QStandardItem* toStdItem(const QJsonObject &jo, QString parent);
    QDomElement jsonToDom(const QString &key, const QJsonValue &value);
    QDomElement jsonToDom(QJsonObject jsondoc, const QString name);
};
#endif // MAINWINDOW_H
