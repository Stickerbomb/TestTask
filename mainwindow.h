#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "xmlparser.h"

#include <QCryptographicHash>
#include <QMainWindow>

class MainWindow : public QMainWindow, private Ui::MainWindow {
		Q_OBJECT

public:
		MainWindow(QWidget *parent = nullptr);

private slots:
		void insertChild();
		void insertColumn();
		void insertRow();
		void removeColumn();
		void removeRow();

		void on_open_file_clicked();
        void on_save_file_clicked();
        void fileChecksum();

private:
		QMenu *fileMenu;
		std::unique_ptr<XmlParser> xmlParser;
};

#endif // MAINWINDOW_H
