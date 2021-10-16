#include <iostream>
#include <chrono>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using std::cout;
using std::cerr;
using std::cin;
using std::endl;

std::vector<std::vector<cv::Mat>> image_mean_and_deviation(const cv::Mat &image);
void color_transfer(const cv::Mat& src, const cv::Mat& tgt, cv::Mat& res);

int main(int argc, char **argv) {

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    
    cv::Mat image = cv::imread(argv[1], 1);
    cv::Mat target = cv::imread(argv[2], 1);

    if (image.data == nullptr || target.data == nullptr) { 
        cerr << "Images load not complete, exit!"<< endl;
        return 0;
    }

    cv::Mat src, trans;
    cv::cvtColor(image, src, CV_BGR2Lab);
    cv::cvtColor(src, trans, CV_Lab2BGR);

    cv::Mat res;
    color_transfer(image, target, res);
    
    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_used = std::chrono::duration_cast < std::chrono::duration < double >> (t2 - t1);
    cout << "Algorithms take：" << time_used.count() << " seconds" << endl;

    cv::imshow("source", image);
    cv::imshow("target", target);
    cv::imshow("result", res);
    cv::waitKey(0);
}

std::vector<std::vector<cv::Mat>> image_mean_and_deviation(const cv::Mat &image) {
    std::vector<cv::Mat> lab;
    std::vector<cv::Mat> mean(3), deviation(3);
    std::vector<std::vector<cv::Mat>> mean_deviation;

    cv::split(image, lab);

    // mean_deviation : matrix 3 * 2
    //  +++    mean    deviation
    //      l
    //     a
    //     b   
    for (int i = 0; i < 3; ++i) {
        cv::meanStdDev(lab[i], mean[i], deviation[i]);
        mean_deviation.push_back({mean[i], deviation[i]});
    }
    return mean_deviation;
}


void color_transfer(const cv::Mat& src, const cv::Mat& tgt, cv::Mat& res) {
    cv::Mat tmp_src, tmp_tgt;

    // convert the image from rgb to lab
    cv::cvtColor(src, tmp_src, CV_BGR2Lab);
    cv::cvtColor(tgt, tmp_tgt, CV_BGR2Lab);

    // compulate the mean and deviation
    std::vector<std::vector<cv::Mat>> mean_dev_from_src =  image_mean_and_deviation(tmp_src);
    std::vector<std::vector<cv::Mat>> mean_dev_from_tgt =  image_mean_and_deviation(tmp_tgt);

    // split the target image into 3 channels (l, a, b)
    std::vector<cv::Mat> lab;
    cv::split(tmp_src, lab);

    // map
    for (int i = 0; i != 3; ++i) {
        lab[i] -= mean_dev_from_src[i][0];
        lab[i] *= mean_dev_from_tgt[i][1].at<double>(0, 0) / mean_dev_from_src[i][1].at<double>(0, 0);
        lab[i] += mean_dev_from_tgt[i][0];
    }

    // merge the lab channels and convert back to rgb
    cv::merge(lab, res);
    res.convertTo(res, CV_8U);
    cv::cvtColor(res, res, CV_Lab2BGR);
}





