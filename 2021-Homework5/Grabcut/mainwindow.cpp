#include "mainwindow.h"
#include "ui_mainwindow.h"

const int MAX_WIDTH = 500;
const int margin_x = 10;
const int margin_y = 30;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Grabcut");

    connect(ui->actionNewImage, SIGNAL(triggered()), this, SLOT(loadSourceImage()));
    connect(ui->actionBkdImage, SIGNAL(triggered()), this, SLOT(loadBackgroundImage()));
    connect(ui->actionFore, SIGNAL(triggered()), this, SLOT(extractFore()));
    connect(ui->actionBack, SIGNAL(triggered()), this, SLOT(extractBack()));
    connect(ui->actionMorph, SIGNAL(triggered()), this, SLOT(morphing()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveImage()));

    this->source = nullptr;
    this->source_tmp = nullptr;
    this->foreImage = nullptr;
    this->backImage = nullptr;
    this->background = nullptr;
    this->background_tmp = nullptr;
    this->fusionImage = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::saveImage() {
    if (!(this->cv_foreImage.empty())) {
        cv::imwrite("foreImage.png", this->cv_foreImage);
    }
}


void MainWindow::extractFore() {

    cv::Mat bgmodle, fgmodel;

    cv::Rect rect(this->sp, this->ep);

    mask = cv::Mat(this->cv_source.rows, this->cv_source.cols,CV_8UC1, cv::Scalar(cv::GC_BGD));

    (mask(rect)).setTo(cv::Scalar(cv::GC_PR_FGD) );

    std::cout << this->sp << " " << this->ep << std::endl;

    for (int i = 0; i < this->source_tmp->width(); ++i) {
        for (int j = 0; j < this->source_tmp->height(); ++j) {
            if (this->source_tmp->pixel(i, j) == qRgb(255,255,0)) {
                cv::Point p(cv::Point(i, j));
                cv::circle(mask, p, 1, cv::GC_BGD);
            }
            if (this->source_tmp->pixel(i, j) == qRgb(0, 0, 255)) {
                cv::Point p(cv::Point(i, j));
                cv::circle(mask, p, 1, cv::GC_FGD);
            }
        }
    }



    cv::grabCut(this->cv_source, mask, rect, bgmodle, fgmodel, 10, cv::GC_INIT_WITH_MASK);

    cv::compare(mask, cv::GC_PR_FGD, mask, cv::CMP_EQ);

    for (int i = 0; i < this->source_tmp->width(); ++i) {
        for (int j = 0; j < this->source_tmp->height(); ++j) {
            if (this->source_tmp->pixel(i, j) == qRgb(0, 0, 255)) {
                cv::Point p(cv::Point(i, j));
                cv::circle(mask, p, 1, cv::GC_PR_FGD);
            }
        }
    }
    cv::Mat foreground(this->cv_source.size(), CV_8UC3, cv::Scalar(255, 255, 255));

    this->cv_source.copyTo(foreground, mask);

    cv::imshow("mask",mask);
    //     make foreground transparent
    cv::cvtColor(foreground, foreground, CV_BGR2BGRA);

    for (int i = 0; i < foreground.rows; ++i) {
        for (int j = 0; j < foreground.cols; ++j) {
            cv::Vec4b &fpixel = foreground.at<cv::Vec4b>(i, j);
            if (mask.at<uchar>(i, j) == 0 || mask.at<uchar>(i, j) == 2) {
                fpixel[3] = 0;
            }
        }
    }

    foreground.copyTo(this->cv_foreImage);

//    cv::imwrite("foreImage.png", this->cv_foreImage);
//    cv::imshow("fore",this->cv_foreImage);

    QImage *img = new QImage(cvMat2QImage(this->cv_foreImage, true, true));

    if (this->foreImage) {
        delete this->foreImage;
        this->foreImage = img;
        img = nullptr;
    } else {
        this->foreImage = img;
        img = nullptr;
    }

    update();
}


void MainWindow::extractBack() {

}


void MainWindow::morphing() {

//    cv::GaussianBlur(mask, mask, cv::Size(5, 5), 0);
    cv::GaussianBlur(cv_background_tmp, cv_background_tmp, cv::Size(0, 0), 15);

    cv_fusionImage = cv::Mat::zeros(cv_source.size(), cv_source.type());

    for (int i = 0; i < cv_source.rows; i++) {
        for (int j = 0; j < cv_source.cols; j++) {
            float w1 = mask.at<uchar>(i, j) / 255.0;
            int b = cv_source.at<cv::Vec3b>(i, j)[0];
            int g = cv_source.at<cv::Vec3b>(i, j)[1];
            int r = cv_source.at<cv::Vec3b>(i, j)[2];
            int b1 = cv_background_tmp.at<cv::Vec3b>(i, j)[0];
            int g1 = cv_background_tmp.at<cv::Vec3b>(i, j)[1];
            int r1 = cv_background_tmp.at<cv::Vec3b>(i, j)[2];
            int b3 = (1.0 - w1) * b1 + b * w1;
            int g3 = (1.0 - w1) * g1 + g * w1;
            int r3 = (1.0 - w1) * r1 + r * w1;
            cv_fusionImage.at<cv::Vec3b>(i, j)[0] = static_cast<int>(b3);
            cv_fusionImage.at<cv::Vec3b>(i, j)[1] = static_cast<int>(g3);;
            cv_fusionImage.at<cv::Vec3b>(i, j)[2] = static_cast<int>(r3);;

        }
    }


    QImage *img = new QImage(cvMat2QImage(this->cv_fusionImage, true, true));

    if (this->fusionImage) {
        delete this->fusionImage;
        this->fusionImage = img;
        img = nullptr;
    } else {
        this->fusionImage = img;
        img = nullptr;
    }

    update();
}

void MainWindow::loadSourceImage() {
    this->source = cv_loadSrcImage();
    if (this->source_tmp) {
        delete this->source_tmp;
        this->source_tmp = new QImage(*(this->source));
    } else {
        this->source_tmp = new QImage(*(this->source));
    }
    update();
}


void MainWindow::loadBackgroundImage() {
    this->background = cv_loadBkdImage();
    if (this->background_tmp) {
        delete this->background_tmp;
        this->background_tmp = new QImage(*(this->background));
    } else {
        this->background_tmp = new QImage(*(this->background));
    }
    update();
}

QImage* MainWindow::cv_loadSrcImage() {
    QString imgDir = QFileDialog::getOpenFileName(
                this,
                tr("Open Image"),
                "../images",
                tr("Images (*.png *.xpm *.jpg *.tiff *.bmp *.jpeg)"));
    qDebug() << imgDir << endl;
    std::cout << "use cv to load image " << std::endl;
    std::string imgdir = imgDir.toStdString();

    this->cv_source = cv::imread(imgdir);

    if (cv_source.cols > MAX_WIDTH){
        cv::resize(cv_source, cv_source, cv::Size(MAX_WIDTH, cv_source.rows*MAX_WIDTH/cv_source.cols), 0, 0, CV_INTER_LINEAR);
    }

    cv_source.copyTo(this->cv_tmp);

//    QImage tmp = cvMat2QImage(this->cv_source, true, true);

    QImage *img = new QImage(cvMat2QImage(this->cv_source, true, true));
//    if ((*img).width() > MAX_WIDTH) {
//        *img = img->scaledToWidth(MAX_WIDTH);
//    }
    // *img = img->scaledToWidth(300);
    return img;
}


QImage* MainWindow::cv_loadBkdImage() {
    QString imgDir = QFileDialog::getOpenFileName(
                this,
                tr("Open Image"),
                "../images",
                tr("Images (*.png *.xpm *.jpg *.tiff *.bmp *.jpeg)"));
    qDebug() << imgDir << endl;
    std::cout << "use cv to load image " << std::endl;
    std::string imgdir = imgDir.toStdString();

    this->cv_background = cv::imread(imgdir);

    cv::resize(cv_background, cv_background, cv::Size(cv_source.cols, cv_source.rows), 0, 0, CV_INTER_LINEAR);

    cv_background.copyTo(this->cv_background_tmp);

    QImage *img = new QImage(cvMat2QImage(this->cv_background, true, true));

    return img;
}


void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (this->source)
        painter.drawImage(margin_x, margin_y, *(this->source_tmp));
    if (this->foreImage)
        painter.drawImage(2*margin_x + this->source->width(), margin_y, *(this->foreImage));
    if (this->background)
        painter.drawImage(margin_x, 2*margin_y+this->source->height(), *(this->background_tmp));
    if (this->fusionImage)
        painter.drawImage(2*margin_x + this->source->width(), 2*margin_y+this->source->height(), *(this->fusionImage));
}


void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton){
        ex = event->pos().x(); //鼠标相对于所在控件的位置
        ey = event->pos().y();
        for(int i = this->sx; i < this->ex; i++)
            for(int j = this->sy; j < this->ey; j++) {
                (*(this->source_tmp)).setPixel(i-margin_x, j-margin_y, qRgb(255,0,0));
            }
        if (ex - sx >= 4 && ey - sy >= 4) {
            for(int i = this->sx+1; i < this->source_tmp->width() + margin_x; i++)
                for(int j = this->sy+1; j < this->source_tmp->height() + margin_y; j++) {
                    if ((i < this->ex-1) && (j < this->ey-1) || (i >= this->ex) || (j >= this->ey)) {
                        (*(this->source_tmp)).setPixel(i-margin_x, j-margin_y, QRgb((this->source->pixel(i-margin_x, j-margin_y))));
                    }
                }
        }
    }
    // foreground
    if (event->buttons() & Qt::RightButton && event->modifiers() == Qt::ShiftModifier) {
        int bkd_x = event->pos().x();
        int bkd_y = event->pos().y();
        const int neighbor = 1;
        for(int i = bkd_x-margin_x-neighbor;i < bkd_x - margin_x+neighbor;i++)
            for(int j = bkd_y-margin_y-neighbor;j < bkd_y-margin_y+neighbor;j++)
                (*(this->source_tmp)).setPixel(i, j, qRgb(0, 0, 255)); //blue
    }
    //background
    if (event->buttons() & Qt::RightButton && event->modifiers() == Qt::ControlModifier) {
        int bkd_x = event->pos().x();
        int bkd_y = event->pos().y();
        const int neighbor = 1;
        for(int i = bkd_x-margin_x-neighbor;i < bkd_x - margin_x+neighbor;i++)
            for(int j = bkd_y-margin_y-neighbor;j < bkd_y-margin_y+neighbor;j++)
                (*(this->source_tmp)).setPixel(i, j, qRgb(255,255,0)); //yellow
    }
    update();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        sx = event->pos().x();
        sy = event->pos().y();
    }
    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        ex = event->pos().x();
        ey = event->pos().y();
    }

    this->sp = cv::Point(sx-margin_x, sy-margin_y);
    this->ep = cv::Point(ex-margin_x, ey-margin_y);
    update();
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
