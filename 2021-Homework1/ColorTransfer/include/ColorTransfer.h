#ifndef   COLORTRANSFER_H 
#define   COLORTRANSFER_H 

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using cv::Mat;
using std::vector;

class ColorTransfer
{
public:
    ColorTransfer() = default;
    ColorTransfer(const Mat src, const Mat target)
    {
        src.convertTo(srcImg_32F, CV_32FC3, 1.0f / 255.0f); // convert type for transfer
        target.convertTo(targetImg_32F, CV_32FC3, 1.0f / 255.0f);
    }
    void transfer(Mat &res);

private:
    // imput rgb image
    Mat srcImg_32F;
    Mat targetImg_32F;

    Mat RGBToLab(const Mat m);
    Mat LabToRGB(const Mat m);
    vector<double> computeMeans(const Mat &image);
    vector<double> computeDeviations(const Mat &image, vector<double> means);
};

#endif 
