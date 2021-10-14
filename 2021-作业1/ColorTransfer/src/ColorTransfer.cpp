#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include  "ColorTransfer.h"

using cv::Mat;
using cv::Mat_;
using cv::Vec3f;
using std::vector;

Mat ColorTransfer::RGBToLab(const Mat m)
{
    Mat_<Vec3f> tmp_img = m;
    for (int i = 0; i < tmp_img.rows; ++i)
    {
        for (int j = 0; j < tmp_img.cols; ++j)
        {
            // RGB to LMS
            double L = 0.3811 * tmp_img(i, j)[0] + 0.5783 * tmp_img(i, j)[1] + 0.0402 * tmp_img(i, j)[2];
            double M = 0.1967 * tmp_img(i, j)[0] + 0.7244 * tmp_img(i, j)[1] + 0.0782 * tmp_img(i, j)[2];
            double S = 0.0241 * tmp_img(i, j)[0] + 0.1288 * tmp_img(i, j)[1] + 0.8444 * tmp_img(i, j)[2];
            if (L == 0)
                L = 1;
            if (M == 0)
                M = 1;
            if (S == 0)
                S = 1;
            L = log(L);
            M = log(M);
            S = log(S);

            // LMS to Lab
            tmp_img(i, j)[0] = (L + M + S) / sqrt(3.0);
            tmp_img(i, j)[1] = (L + M - 2 * S) / sqrt(6.0);
            tmp_img(i, j)[2] = (L - M) / sqrt(2.0);
        }
    }
    return tmp_img;
}

Mat ColorTransfer::LabToRGB(const Mat m)
{
    Mat_<Vec3f> tmp_img = m;
    for (int i = 0; i < tmp_img.rows; ++i)
    {
        for (int j = 0; j < tmp_img.cols; ++j)
        {
            // Lab to LMS
            double L = tmp_img(i, j)[0] / sqrt(3.0) + tmp_img(i, j)[1] / sqrt(6.0) + tmp_img(i, j)[2] / sqrt(2.0);
            double M = tmp_img(i, j)[0] / sqrt(3.0) + tmp_img(i, j)[1] / sqrt(6.0) - tmp_img(i, j)[2] / sqrt(2.0);
            double S = tmp_img(i, j)[0] / sqrt(3.0) - 2 * tmp_img(i, j)[1] / sqrt(6.0);

            L = exp(L);
            M = exp(M);
            S = exp(S);

            // LMS to RGB
            tmp_img(i, j)[0] = 4.4679 * L - 3.5873 * M + 0.1193 * S;
            tmp_img(i, j)[1] = -1.2186 * L + 2.3809 * M - 0.1624 * S;
            tmp_img(i, j)[2] = 0.0497 * L - 0.2439 * M + 1.2045 * S;
        }
    }
    return tmp_img;
}

vector<double> ColorTransfer::computeMeans(const Mat &image)
{
    vector<double> means(3), sum(3);

    for (int i = 0; i < image.rows; ++i)
        for (int j = 0; j < image.cols; ++j)
            for (int k = 0; k < 3; k++)
                sum[k] += image.at<Vec3f>(i, j)[k];

    for (int i = 0; i < 3; ++i)
        means[i] = sum[i] / (image.rows * image.cols);

    return means;
}

vector<double> ColorTransfer::computeDeviations(const Mat &image, vector<double> means)
{
    vector<double> Deviations(3), sum(3);

    // 3 channnels
    for (int i = 0; i < image.rows; ++i)
        for (int j = 0; j < image.cols; ++j)
            for (int k = 0; k < 3; ++k)
                sum[k] += abs(image.at<Vec3f>(i, j)[k] - means[k]);

    for (int i = 0; i < 3; i++)
        Deviations[i] = sqrt(sum[i] / (image.rows * image.cols));

    return Deviations;
}

void ColorTransfer::transfer(Mat &res)
{
    res = srcImg_32F; // init the result by src
    Mat srcImg_Lab = RGBToLab(srcImg_32F);
    Mat tgtImg_Lab = RGBToLab(targetImg_32F);
    vector<double> srcMeans = computeMeans(srcImg_Lab);
    vector<double> tgtMeans = computeMeans(tgtImg_Lab);
    vector<double> srcDeviations = computeDeviations(srcImg_Lab, srcMeans);
    vector<double> tgtDeviations = computeDeviations(tgtImg_Lab, tgtMeans);

    vector<double> cvt_coef(3);
    for (int k = 0; k < 3; ++k)
        cvt_coef[k] = tgtDeviations[k] / srcDeviations[k];

    for (int i = 0; i < res.rows; ++i)
        for (int j = 0; j < res.cols; ++j)
            for (int k = 0; k < 3; ++k)
                res.at<Vec3f>(i, j)[k] = cvt_coef[k] * (res.at<Vec3f>(i, j)[k] - srcMeans[k]) + tgtMeans[k];

    res = LabToRGB(res);
    Mat tmp = res.clone();  // deep clone
    tmp.convertTo(res, CV_8U, 255.0f, 1.0f / 255.0f);   // recover th image type
}