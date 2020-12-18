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

private:
    QDomDocument document;
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    void Read(QString _noed);
    void Write(QString _root);
    void Show(const QDomNode &_elem, QStandardItem *subModel);
};
#endif // MAINWINDOW_H
