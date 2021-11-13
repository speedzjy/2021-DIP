#include "mainwindow.h"
#include "ui_mainwindow.h"

const int MAX_WIDTH = 500;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("ImagePyramid");

    ui->psnrlineEdit->setReadOnly(true);
    ui->ssimlineEdit->setReadOnly(true);

    connect(ui->actionNewImage, SIGNAL(triggered()), this, SLOT(loadSourceImage()));
    connect(ui->actionPool, SIGNAL(triggered()), this, SLOT(downSamplePool()));
    connect(ui->actionGauss, SIGNAL(triggered()), this, SLOT(downSampleGauss()));
    connect(ui->actionLaplace, SIGNAL(triggered()), this, SLOT(upSampleLaplace()));
    connect(ui->actionNN, SIGNAL(triggered()), this, SLOT(upSampleNN()));
    connect(ui->actionLinear, SIGNAL(triggered()), this, SLOT(upSampleLinear()));
    connect(ui->actionGenDiff, SIGNAL(triggered()), this, SLOT(genDiff()));

    this->source = nullptr;
    this->source_tmp = nullptr;
    this->downSampleImage = nullptr;
    this->upSampleImage = nullptr;
    this->diffImage = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::downSamplePool() {

    cv_dtmp = maxPooling(cv_tmp);

//    cv::imshow("down_max_pooling", cv_dtmp);
//    cv::waitKey(0);

    QImage *img = new QImage(cvMat2QImage(this->cv_dtmp, true, true));
    if ((*img).width() > MAX_WIDTH/2) {
        *img = img->scaledToWidth(MAX_WIDTH/2);
    }

    if (this->downSampleImage) {
        delete this->downSampleImage;
        this->downSampleImage = img;
        img = nullptr;
    } else {
        this->downSampleImage = img;
        img = nullptr;
    }
    update();
}

void MainWindow::downSampleGauss() {
    cv::pyrDown(cv_tmp, cv_dtmp);

//    cv::imshow("down_Gauss", cv_dtmp);
//    cv::waitKey(0);

    QImage *img = new QImage(cvMat2QImage(this->cv_dtmp, true, true));
    if ((*img).width() > MAX_WIDTH/2) {
        *img = img->scaledToWidth(MAX_WIDTH/2);
    }

    if (this->downSampleImage) {
        delete this->downSampleImage;
        this->downSampleImage = img;
        img = nullptr;
    } else {
        this->downSampleImage = img;
        img = nullptr;
    }
    update();
}

void MainWindow::upSampleLaplace() {
    if (this->downSampleImage) {
        cv::pyrUp(cv_dtmp, cv_utmp);

//        cv::imshow("up_laplace", cv_utmp);
//        cv::waitKey(0);

        QImage *img = new QImage(cvMat2QImage(this->cv_utmp, true, true));
        if ((*img).width() > MAX_WIDTH) {
            *img = img->scaledToWidth(MAX_WIDTH);
        }

        if (this->upSampleImage) {
            delete this->upSampleImage;
            this->upSampleImage = img;
            img = nullptr;
        } else {
            this->upSampleImage = img;
            img = nullptr;
        }
    }
    update();
}

void MainWindow::upSampleNN() {
    if (this->downSampleImage) {
        cv::resize(cv_dtmp, cv_utmp, cv::Size(2*cv_dtmp.cols, 2*cv_dtmp.rows), 0, 0, CV_INTER_NN);

//        cv::imshow("up_NN", cv_utmp);
//        cv::waitKey(0);

        QImage *img = new QImage(cvMat2QImage(this->cv_utmp, true, true));
        if ((*img).width() > MAX_WIDTH) {
            *img = img->scaledToWidth(MAX_WIDTH);
        }

        if (this->upSampleImage) {
            delete this->upSampleImage;
            this->upSampleImage = img;
            img = nullptr;
        } else {
            this->upSampleImage = img;
            img = nullptr;
        }
    }
    update();
}

void MainWindow::upSampleLinear() {
    if (this->downSampleImage) {
        cv::resize(cv_dtmp, cv_utmp, cv::Size(2*cv_dtmp.cols, 2*cv_dtmp.rows), 0, 0, CV_INTER_LINEAR);

//        cv::imshow("up_Bi_linear", cv_utmp);
//        cv::waitKey(0);

        QImage *img = new QImage(cvMat2QImage(this->cv_utmp, true, true));
        if ((*img).width() > MAX_WIDTH) {
            *img = img->scaledToWidth(MAX_WIDTH);
        }

        if (this->upSampleImage) {
            delete this->upSampleImage;
            this->upSampleImage = img;
            img = nullptr;
        } else {
            this->upSampleImage = img;
            img = nullptr;
        }
    }
    update();
}

void MainWindow::genDiff() {
    if (this->upSampleImage) {
        cv::absdiff(this->cv_tmp, this->cv_utmp, this->cv_diffImage);

//        cv::imshow("diff", cv_diffImage);
//        cv::waitKey(0);

        QImage *img = new QImage(cvMat2QImage(this->cv_diffImage, true, true));
        if ((*img).width() > MAX_WIDTH) {
            *img = img->scaledToWidth(MAX_WIDTH);
        }

        if (this->diffImage) {
            delete this->diffImage;
            this->diffImage = img;
            img = nullptr;
        } else {
            this->diffImage = img;
            img = nullptr;
        }

        double psnr = getPSNR(this->cv_tmp, this->cv_utmp);
        cv::Scalar ssim = getMSSIM(this->cv_tmp, this->cv_utmp);
        double ssim_avg = (ssim.val[0] + ssim.val[1] +ssim.val[2]) / 3;
        std::cout << "PSNR: " << psnr << std::endl;
        std::cout << "SSIM: " << ssim << std::endl;
        ui->psnrlineEdit->setText(QString::number(psnr));
        ui->ssimlineEdit->setText(QString::number(ssim_avg));
    }
    update();
}

cv::Mat MainWindow::maxPooling(const cv::Mat img){
  int height = img.rows;
  int width = img.cols;
  int channel = img.channels();

  int r = 2;
  double v = 0;

  // prepare output
  cv::Mat out = cv::Mat::zeros(height/2, width/2, CV_8UC3);

  std::cout << "hh" << std::endl;

  for (int y = 0, p = 0; y < height; y+=r, p++){
    for (int x = 0, q = 0; x < width; x+=r, q++){
      for (int c = 0; c < channel; c++){
        v = 0;
        for (int dy = 0; dy < r; dy++){
          for (int dx = 0; dx < r; dx++){
            v = fmax(img.at<cv::Vec3b>(y + dy, x + dx)[c], v);
          }
        }

        out.at<cv::Vec3b>(p, q)[c] = (uchar)v;
//        for (int dy = 0; dy < r; dy++){
//          for (int dx = 0; dx < r; dx++){
//            out.at<cv::Vec3b>(y + dy, x + dx)[c] = (uchar)v;
//          }
//        }
      }
      }
    }
  return out;
}

double MainWindow::getPSNR(const cv::Mat &I1, const cv::Mat &I2)
{
    cv::Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2
    cv::Scalar s = sum(s1);         // sum elements per channel
    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels
    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double  mse =sse /(double)(I1.channels() * I1.total());
        double psnr = 10.0*log10((255*255)/mse);
        return psnr;
    }
}

cv::Scalar MainWindow::getMSSIM(const cv::Mat &i1, const cv::Mat &i2)
{
    using cv::Mat;
    using cv::Size;
    using cv::Scalar;

    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d     = CV_32F;
    Mat I1, I2;
    i1.convertTo(I1, d);           // cannot calculate on one byte large values
    i2.convertTo(I2, d);
    Mat I2_2   = I2.mul(I2);        // I2^2
    Mat I1_2   = I1.mul(I1);        // I1^2
    Mat I1_I2  = I1.mul(I2);        // I1 * I2
    /*************************** END INITS **********************************/
    Mat mu1, mu2;   // PRELIMINARY COMPUTING
    GaussianBlur(I1, mu1, Size(11, 11), 1.5);
    GaussianBlur(I2, mu2, Size(11, 11), 1.5);
    Mat mu1_2   =   mu1.mul(mu1);
    Mat mu2_2   =   mu2.mul(mu2);
    Mat mu1_mu2 =   mu1.mul(mu2);
    Mat sigma1_2, sigma2_2, sigma12;
    GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;
    GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;
    GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;
    Mat t1, t2, t3;
    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
    Mat ssim_map;
    divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;
    Scalar mssim = mean( ssim_map ); // mssim = average of ssim map
    return mssim;
}

void MainWindow::loadSourceImage() {
    this->source = cv_loadImage();
    if (this->source_tmp) {
        delete this->source_tmp;
        this->source_tmp = new QImage(*(this->source));
    } else {
        this->source_tmp = new QImage(*(this->source));
    }
    update();
}

QImage* MainWindow::cv_loadImage() {
    QString imgDir = QFileDialog::getOpenFileName(
                this,
                tr("Open Image"),
                "../images",
                tr("Images (*.png *.xpm *.jpg *.tiff *.bmp)"));
    qDebug() << imgDir << endl;
    std::cout << "use cv to load image " << std::endl;
    std::string imgdir = imgDir.toStdString();

    this->cv_source = cv::imread(imgdir);
    cv_source.copyTo(this->cv_tmp);

//    QImage tmp = cvMat2QImage(this->cv_source, true, true);

    QImage *img = new QImage(cvMat2QImage(this->cv_source, true, true));
    if ((*img).width() > MAX_WIDTH) {
        *img = img->scaledToWidth(MAX_WIDTH);
    }
    // *img = img->scaledToWidth(300);
    return img;
}

QImage* MainWindow::loadImage() {
    QString imgDir = QFileDialog::getOpenFileName(
                this,
                tr("Open Image"),
                "../images",
                tr("Images (*.png *.xpm *.jpg *.tiff *.bmp)"));
    qDebug() << imgDir << endl;

    std::string imgdir = imgDir.toStdString();

    QImage *img = new QImage(imgDir);
    *img = img->scaledToWidth(300);
    return img;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (this->source)
        painter.drawImage(10, 50, *(this->source_tmp));
    if (this->downSampleImage)
        painter.drawImage(20+this->source->width(), 50, *(this->downSampleImage));
    if (this->upSampleImage)
        painter.drawImage(30+this->source->width()+this->downSampleImage->width(), 50, *(this->upSampleImage));
    if (this->diffImage)
        painter.drawImage(40+this->source->width()+
                          this->downSampleImage->width()+
                          this->upSampleImage->width(),
                          50,
                          *(this->diffImage));
}

QImage MainWindow::cvMat2QImage(const cv::Mat& mat, bool clone, bool rb_swap)
{
    const uchar *pSrc = (const uchar*)mat.data;
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        //QImage image(mat.cols, mat.rows, QImage::Format_Grayscale8);
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        if(clone) return image.copy();
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        if(clone)
        {
            if(rb_swap) return image.rgbSwapped();
            return image.copy();
        }
        else
        {
            if(rb_swap)
            {
                cv::cvtColor(mat, mat, CV_BGR2RGB);
            }
            return image;
        }

    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        if(clone) return image.copy();
        return image;
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

cv::Mat MainWindow::QImage2cvMat(QImage &image, bool clone, bool rb_swap)
{
    cv::Mat mat;
    //qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void *)image.constBits(), image.bytesPerLine());
        if(clone)  mat = mat.clone();
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void *)image.constBits(), image.bytesPerLine());
        if(clone)  mat = mat.clone();
        if(rb_swap) cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void *)image.bits(), image.bytesPerLine());
        if(clone)  mat = mat.clone();
        break;
    }
    return mat;
}
