#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    get_license_plate_area("C:/Users/houxia2x/Desktop/Asa/opencv/qt_opencv_demo/car.jpg");
}

MainWindow::~MainWindow()
{
}

void MainWindow::get_license_plate_area(string path)
{
    /*加载图片*/
    Mat srcImage = imread(path);
    if(srcImage.empty())
    {
        cout<<"load pic failed";
        return;
    }
    Mat dealImage = srcImage.clone();

    /*高斯模糊（图像平滑处理）*/
    GaussianBlur(srcImage,dealImage,Size(3,3),0);
    //    imshow("gaussion image",dealImage);

    /*边缘检查*/
    Mat h_filler,v_filler,abs_h_filler,abs_v_filler;
    Sobel(dealImage,h_filler,CV_16S,1,0,3,1,0,BORDER_DEFAULT);      //横向检查
    convertScaleAbs(h_filler,abs_h_filler);
    Sobel(dealImage,v_filler,CV_16S,0,1,3,1,0,BORDER_DEFAULT);      //纵向检查
    convertScaleAbs(v_filler,abs_v_filler);
    addWeighted(abs_h_filler,0.5,abs_v_filler,0.5,0,dealImage);     //合并检查结果
    imshow("sobel image",dealImage);

    /*图像二值化处理*/
    cvtColor(dealImage,dealImage,COLOR_BGR2GRAY);           //转为灰度图像
    threshold(dealImage,dealImage,95,255,THRESH_BINARY);    //二值化
    //    imshow("threshold image",dealImage);

    /*图像补全处理*/
    Mat element = getStructuringElement(MORPH_RECT,Size(25,25));    //核阵列
    vector<vector<Point>> vec_count;
    morphologyEx(dealImage,dealImage,MORPH_CLOSE,element);          //闭运算（膨胀--腐蚀）
    morphologyEx(dealImage,dealImage,MORPH_OPEN,element);           //开运算（腐蚀--膨胀）
    //    imshow("morphology open image",dealImage);

    /*获取车牌区域(截取车牌范围的算法可自行判断设计)*/
    vector<vector<Point>> vec_area;
    findContours(dealImage,vec_area,RETR_EXTERNAL,CHAIN_APPROX_NONE);
    vector<Point> max_point_area;
    size_t num_max = 0;
    Point2f tmp_rect[4];
    for(size_t i = 0 ; i < vec_area.size() ; ++i)       //获取最大面积的区域，即车牌区域
    {
        minAreaRect(vec_area[i]).points(tmp_rect);
        double width = powf((tmp_rect[0].x - tmp_rect[1].x), 2) + powf((tmp_rect[0].y - tmp_rect[1].y), 2);
        width = sqrt(width);
        double height = powf((tmp_rect[0].x - tmp_rect[3].x), 2) + powf((tmp_rect[0].y - tmp_rect[3].y), 2);
        height = sqrt(height);
        if(height*width > num_max)
        {
            num_max = height*width;
            max_point_area = vec_area[i];
        }
    }
    if(num_max > 0)         //是否获取到区域
    {
        /*描绘最大区域的轮廓*/
        Mat carPlateAreaImage = srcImage.clone();
        for(size_t i = 0 ; i < max_point_area.size() ; ++i)
        {
            circle(carPlateAreaImage,max_point_area.at(i),2,Scalar(255,0,0));
        }

        /*描绘最大区域的矩形形态*/
        Point2f max_rect_corner_point[4];
        RotatedRect max_rect = minAreaRect(max_point_area);
        max_rect.points(max_rect_corner_point);
        for(size_t j = 0 ; j < 4 ; ++j)
        {
            line(carPlateAreaImage,max_rect_corner_point[j],max_rect_corner_point[(j+1)%4],Scalar(0,0,255),3);
        }
        //        imshow("carPlateAreaImage image",carPlateAreaImage);

        Mat carPlateImage = Mat(srcImage,max_rect.boundingRect());
        imshow("carPlateImage image",carPlateImage);
    }else{
        cout<<"Fail for fetch area of car plate";
    }
}
