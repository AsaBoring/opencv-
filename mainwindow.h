#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "opencv2/opencv.hpp"

#include <iostream>
#include <QMainWindow>

using namespace std;
using namespace cv;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void get_license_plate_area(string path);
    ~MainWindow();
};
#endif // MAINWINDOW_H
