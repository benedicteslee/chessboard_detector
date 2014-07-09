#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencvbook.cpp"
#include "Line.h"
#include "square.h"
#include "preprocess.h"
#include "boarddetector.h"
#include "cvutils.h"
#include "typedefs.h"
#include "matrix.h"
#include <vector>
#include <numeric>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->pushButton_2->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), "/Users/benedicte/Dropbox/kings/thesis/images",
                                                    tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));

    img_rgb = cv::imread(fileName.toStdString().data());
    if (img_rgb.data){
        //ui->pushButton_2->setEnabled(true);

        // Convert image to graylevel and normalize
        cv::cvtColor(img_rgb, img_gray, CV_RGB2GRAY);
        cv::normalize(img_gray, img_gray, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (ui->UseDefaultImage->isChecked()){
        img_rgb = cv::imread("/Users/benedicte/Dropbox/kings/thesis/images/checkers1.jpg");
        cv::cvtColor(img_rgb, img_gray, CV_RGB2GRAY);
        cv::normalize(img_gray, img_gray, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    }


    cv::Mat img;

    cv::resize(img_gray, img, cv::Size(1000, img_gray.rows * 1000/img_gray.cols));
    cv::imshow("downsized", img);
    cv::waitKey();

    // Find houghlines
    std::vector<Line> houghlines;
    
    Preprocess prep = Preprocess(img);
    prep.getLines(houghlines);

    prep.showCanny();
    prep.showHoughlines();

    // chessboard detector
    BoardDetector cbd = BoardDetector(img, houghlines);



    // add lines to image
    //cv::Mat img_rgb(image.size(), CV_8UC3);
    //cv::cvtColor(image, img_rgb, CV_GRAY2RGB);

    /*
    Lines hlinesSorted = cbd.get_hlinesSorted();
    Lines vlinesSorted = cbd.get_vlinesSorted();

     for( size_t i = 0; i < hlinesSorted.size(); i++ )
    {
        std::vector<cv::Point> l = hlinesSorted.at(i).points;
        cv::line(img_rgb, l[0], l[1], cv::Scalar(255,0,0), 1, CV_AA);
        cv::imshow("lines", img_rgb);
        cv::waitKey(1);
    }

    for( size_t i = 0; i < vlinesSorted.size(); i++ )
    {
        std::vector<cv::Point> l = vlinesSorted.at(i).points;
        cv::line(img_rgb, l[0], l[1], cv::Scalar(255,0,0), 1, CV_AA);
        cv::imshow("lines", img_rgb);
        cv::waitKey(1);
    }

    */
    /*
    // create binary image
    cv::Mat binary;
    cv::threshold(img_gray, binary, 90, 255, 3); // TODO threshold hardcoded to 90. Base on mean color of squares?
    cv::imshow("binary", binary);
    cv::waitKey(0);
    */
    /*
     //PLOT POSSIBLE SQUARES
    cv::RNG rng = cv::RNG(1234);
    for (size_t i = 0; i < possibleSquares.size(); ++i) {
        cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        cv::fillConvexPoly(image, possibleSquares.at(i).getCornerpoints(), col);
        cv::imshow("poly", image);
        cv::waitKey(1);
    }
    cv::waitKey();
    */

    /*
    //PLOT POSSIBLE SQUARES
   cv::RNG rng = cv::RNG(1234);
   std::vector<Squares> possibleSquares2 = cbd.getPossibleSquares2();
   for (size_t i = 0; i < possibleSquares2.size(); i++) {
       Squares row = possibleSquares2.at(i);
       for (size_t j = 0; j < row.size(); j++){
       cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
       cv::fillConvexPoly(image, row.at(j).getCornerpoints(), col);
       cv::imshow("poly", image);
       cv::waitKey(2);
       }
   }
   cv::waitKey();
   */


    // look at intersections

    //Corners corners = cbd.getCorners();

    /*
    for (size_t i = 0; i < corners.size(); ++i) {
        cv::imshow("corner", corners.at(i).getArea());
        cv::waitKey();
    }
    cv::waitKey();
*/

 /*
    // add circles to image
    //cv::RNG rng = cv::RNG(1234);
    cv::Scalar col = cv::Scalar(0,255,0);
    for (size_t i=0; i<points.size();i++){
        //cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        cv::circle(img_rgb, points.at(i),5, col,1);
        cv::imshow("Intersections", img_rgb);
        //std::cout << newpoints.at(i) << std::endl;
        //cv::waitKey(2);
    }
    cv::imshow("Intersections", img_rgb);
   */
    //cv::destroyAllWindows();
}

void MainWindow::on_pushButton_3_clicked()
{

    matrix<int> m1, m2, m3;

    std::vector<int> row{1, 1, 1};
    m1.addRow(row);
    m2.addRow(row);

    m3 = m1 + m2;
    std::vector<int> v{0,1,2,3,4};
    std::vector<int> vv(&v[1], &v[5]);
    for (size_t i = 0; i < vv.size(); i++){
        std::cout << vv[i] << std::endl;
    }


}
