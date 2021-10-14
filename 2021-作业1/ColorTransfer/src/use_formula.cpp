#include <iostream>
#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include  "ColorTransfer.h"

using cv::Mat;
using cv::imread;
using cv::imshow;
using cv::waitKey;
using cv::imwrite;

int main(int argc, char **argv)
{
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

    Mat source = cv::imread(argv[1], 1);
    Mat target = imread(argv[2], 1);

    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_read = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "read images take：" << time_read.count() << " seconds" << std::endl;

    if (source.data == nullptr || target.data == nullptr) { 
        std::cerr << "Images load not complete, exit!"<< std::endl;
        return 0;
    }

    ColorTransfer clt(source, target);

    Mat result;
    clt.transfer(result);

    std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();

    std::chrono::duration<double> time_algo = std::chrono::duration_cast<std::chrono::duration<double>>(t3 - t2);
    std::cout << "Algorithms take：" << time_algo.count() << " seconds" << std::endl;

    std::chrono::duration<double> time_total = std::chrono::duration_cast<std::chrono::duration<double>>(t3 - t1);
    std::cout << "Total take：" << time_total.count() << " seconds" << std::endl;

    imshow("source", source);
    imshow("target", target);
    imshow("result", result);

    imwrite("result.jpg", result);

    waitKey(0);

    return 0;
}
