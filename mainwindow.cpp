#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Eigen/Dense>
#include <math.h>
#include<fstream>
#include<vector>

using namespace std;
using namespace Eigen;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionNew_triggered()
{
    currentFile.clear();

    //https://stackoverflow.com/questions/8678684/access-the-widget-of-a-tab-in-a-qtabwidget
    QTextEdit* pTextEdit = NULL;
    QWidget* pWidget= ui->tab_text; // for the second tab
    // You can use metaobject to get widget type or qobject_cast
    if (pWidget->metaObject()->className() == "QTextEdit")
        pTextEdit = (QTextEdit*)pWidget;
    else
    {
        QList<QTextEdit *> allTextEdits = pWidget->findChildren<QTextEdit *>();
        if (allTextEdits.count() != 1)
        {
            QMessageBox::warning(this, "Warning", "shit's on fire");
            return;
        }
        pTextEdit = allTextEdits[0];
    }

    // Do whatever you want with it...
    pTextEdit->setText(QString());
}

void MainWindow::on_actionOpen_triggered()
{

    // Opens a dialog that allows you to select a file to open
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");

    // An object for reading and writing files
    QFile file(fileName);

    // Store the currentFile name
    currentFile = fileName;

    // Try to open the file as a read only file if possible or display a
    // warning dialog box
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }

    // Set the title for the window to the file name
    setWindowTitle(fileName);

    // Interface for reading text
    QTextStream in(&file);

    // Copy text in the string
    QString text = in.readAll();

    // Put the text in the textEdit widget
    ui->textEdit->setText(text);

    // Close the file
    file.close();

}

void MainWindow::on_actionSave_text_triggered()
{
    // Opens a dialog for saving a file
    QString fileName = QFileDialog::getSaveFileName(this, "Save as");

    // An object for reading and writing files
    QFile file(fileName);

    // Try to open a file with write only options
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }

    // Store the currentFile name
    currentFile = fileName;

    // Set the title for the window to the file name
    setWindowTitle(fileName);

    // Interface for writing text
    QTextStream out(&file);

    // Copy text in the textEdit widget and convert to plain text
    QString text = ui->textEdit->toPlainText();

    // Output to file
    out << text;

    // Close the file
    file.close();
}

void MainWindow::on_actionSave_text_2_triggered()
{
    //QString fileName = QFileDialog::getSaveFileName(this, "Save");
    if(currentFile==""){
        currentFile="temp.csv";
    }
    QString  fileName = currentFile;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    currentFile = fileName;
    setWindowTitle(fileName);
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.close();
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}

void MainWindow::on_actionCut_triggered()
{
    ui->textEdit->cut();
}

void MainWindow::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}

void MainWindow::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}

void MainWindow::on_actionPlot_current_data_triggered()
{
    //##################################################################
    if(currentFile==""){
        currentFile="temp.csv";
    }
    QString  fileName = currentFile;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }

    setWindowTitle(fileName);
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.close();

    //##################################################################
    MatrixXd matrix;// matrix to be loaded from a file
    vector<double> matrixEntries;// load the matrix from the file
    ifstream matrixDataFile(currentFile.toStdString());// in this object we store the data from the matrix
    string matrixRowString;// this variable is used to store the row of the matrix that contains commas
    string matrixEntry;// this variable is used to store the matrix entry;
    int matrixRowNumber = 0;// this variable is used to track the number of rows

    while (getline(matrixDataFile, matrixRowString)) // here we read a row by row of matrixDataFile and store every line into the string variable matrixRowString
    {
        stringstream matrixRowStringStream(matrixRowString); //convert matrixRowString that is a string to a stream variable.

        while (getline(matrixRowStringStream, matrixEntry, ',')) // here we read pieces of the stream matrixRowStringStream until every comma, and store the resulting character into the matrixEntry
        {
            matrixEntries.push_back(stod(matrixEntry));   //here we convert the string to double and fill in the row vector storing all the matrix entries
        }
        matrixRowNumber++; //update the column numbers
    }

    matrix = Map<Matrix<double, Dynamic, Dynamic, RowMajor>>(matrixEntries.data(), matrixRowNumber, matrixEntries.size() / matrixRowNumber);

    //##################################################################
    int xColumn = ui->xAxis->value();
    int yColumn = ui->yAxis->value();
    VectorXd xValues, yValues;

    xValues = matrix.col(xColumn);
    yValues = matrix.col(yColumn);

    // convert the Eigen objects into the std::vector form
    // .data() returns the pointer to the first memory location of the first entry of the stored object
    // https://eigen.tuxfamily.org/dox/group__TopicStorageOrders.html
    std::vector<double> xValuesStdVector(xValues.data(), xValues.data() + xValues.rows() * xValues.cols());
    std::vector<double> yValuesStdVector(yValues.data(), yValues.data() + yValues.rows() * yValues.cols());

    //convert the std::vector objects to the Qt QVector form
    QVector<double> xValuesQVector = QVector<double>::fromStdVector(xValuesStdVector);
    QVector<double> yValuesQVector = QVector<double>::fromStdVector(yValuesStdVector);

    // this is necessary for seting the axes limits
    double x_maxValue=xValues.maxCoeff();
    double x_minValue=xValues.minCoeff();

    // this is necessary for seting the axes limits
    double y_maxValue=yValues.maxCoeff();
    double y_minValue=yValues.minCoeff();


    QCustomPlot *customPlot=ui->widgetGraph;
    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->addGraph();
    customPlot->graph(0)->setData(xValuesQVector, yValuesQVector);
    customPlot->graph(1)->setData(xValuesQVector, yValuesQVector);

    QPen pen0;
    pen0.setColor(datacolor);
    if(ui->checkBox_graphconnect->isChecked()){
        switch(ui->comboBox_data->currentIndex()){
        case 0:
            pen0.setStyle(Qt::SolidLine);
            break;
        case 1:
            pen0.setStyle(Qt::DashLine);
            break;
        case 3:
            pen0.setStyle(Qt::DotLine);
            break;
        case 4:
            pen0.setStyle(Qt::DashDotLine);
            break;
        case 5:
            pen0.setStyle(Qt::DashDotDotLine);
            break;
        }
    } else {
        pen0.setStyle(Qt::NoPen);
    }
    pen0.setWidthF(ui->spinBox_datawidth->value());
    customPlot->graph(0)->setPen(pen0);

    customPlot->graph(1)->setPen(datacolor);
    customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, ui->spinBox_pointswidth->value()));

    //##################################################################
    //https://www.bragitoff.com/2015/09/c-program-to-linear-fit-the-data-using-least-squares-method/
    double a,b;
    if(ui->checkBox_regression->isChecked()) {
        int i, n;
        n = xValuesStdVector.size();


        VectorXd x;
        x.setLinSpaced(1000,x_minValue,x_maxValue);
        std::vector<double> xn(x.data(), x.data() + x.rows() * x.cols());

        double xsum=0,x2sum=0,ysum=0,xysum=0;                //variables for sums/sigma of xi,yi,xi^2,xiyi etc
        for (i=0;i<n;i++)
        {
            xsum=xsum+xValuesQVector[i];                        //calculate sigma(xi)
            ysum=ysum+yValuesQVector[i];                        //calculate sigma(yi)
            x2sum=x2sum+pow(xValuesQVector[i],2);                //calculate sigma(x^2i)
            xysum=xysum+xValuesQVector[i]*yValuesQVector[i];                    //calculate sigma(xi*yi)
        }

        a=(n*xysum-xsum*ysum)/(n*x2sum-xsum*xsum);            //calculate slope
        b=(x2sum*ysum-xsum*xysum)/(x2sum*n-xsum*xsum);            //calculate intercept

        vector<double> y_fit(1000);                        //an array to store the new fitted values of y
        i=0;
        for (auto& elem : y_fit)
        {
            elem = a*xn[i]+b;
            i++;
        }
        /*
        for (i=0;i<n;i++){
            y_fit[i]=a*xn[i]+b;                  //to calculate y(fitted) at given x points
        }*/

        QVector<double> xnw = QVector<double>::fromStdVector(xn);
        QVector<double> ynw = QVector<double>::fromStdVector(y_fit);
        customPlot->addGraph();
        QPen pen0;
        pen0.setColor(regcolor);
        switch(ui->comboBox_reg->currentIndex()){
        case 0:
            pen0.setStyle(Qt::SolidLine);
            break;
        case 1:
            pen0.setStyle(Qt::DashLine);
            break;
        case 3:
            pen0.setStyle(Qt::DotLine);
            break;
        case 4:
            pen0.setStyle(Qt::DashDotLine);
            break;
        case 5:
            pen0.setStyle(Qt::DashDotDotLine);
            break;
        }
        pen0.setWidthF(ui->spinBox_regwidth->value());
        customPlot->graph(2)->setPen(pen0);
        customPlot->graph(2)->setData(xnw, ynw);

    }

    //##################################################################
    customPlot->rescaleAxes();
    // set the title
    customPlot->xAxis2->setVisible(true);
    customPlot->xAxis2->setLabel(ui->lineEdit_title->text());
    // give the axes some labels:
    customPlot->xAxis->setLabel(ui->lineEdit_xaxis->text());
    customPlot->yAxis->setLabel(ui->lineEdit_yaxis->text());
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(x_minValue-0.1*abs(x_minValue), x_maxValue+0.1*abs(x_maxValue));
    customPlot->yAxis->setRange(y_minValue-0.1*abs(y_minValue), y_maxValue+0.1*abs(y_maxValue));
    // set legend
    if(ui->checkBox_legend->isChecked()){
        char buffer[100];
        string s = ui->lineEdit_xaxis->text().toStdString();
        char c[s.size() + 1];
        strcpy(c, s.c_str());

        string s2 = ui->lineEdit_yaxis->text().toStdString();
        char c2[s2.size() + 1];
        strcpy(c2, s2.c_str());

        std::snprintf(buffer, sizeof(buffer), "%s = %.3f * %s + (%.3f)", c2, a, c, b);
        customPlot->xAxis2->setLabel(buffer);
    }

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot->replot();
}

void MainWindow::on_pushButton_clicked()
{
    QColor color = QColorDialog::getColor(Qt::blue, this, "Choose color");
    if(color.isValid()){
        datacolor = color;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QColor color = QColorDialog::getColor(Qt::red, this, "Choose color");
    if(color.isValid()){
        regcolor = color;
    }
}

