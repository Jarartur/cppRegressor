#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QColorDialog>
#include <QColor>

#include "qcustomplot.h"

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
    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_text_triggered();

    void on_actionSave_text_2_triggered();

    void on_actionExit_triggered();

    void on_actionCopy_triggered();

    void on_actionCut_triggered();

    void on_actionPaste_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionPlot_current_data_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_actionSave_current_plot_to_file_triggered();

private:
    Ui::MainWindow *ui;
    QString currentFile = "";
    QColor datacolor = Qt::blue;
    QColor regcolor = Qt::red;
    QCustomPlot *customPlot;
    bool first_plot = false;
    bool reg_fit_calc = false;

};
#endif // MAINWINDOW_H
