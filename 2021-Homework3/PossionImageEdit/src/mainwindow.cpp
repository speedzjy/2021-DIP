#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "possionedit.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("PossionImageEdit");
    this->setWindowIcon(QIcon("../resources/123.jpg"));
    connect(ui->srcButton, SIGNAL(clicked()), this, SLOT(loadSourceImage()));
    connect(ui->bkdButton, SIGNAL(clicked()), this, SLOT(loadBackgroundImage()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearDraw()));
    connect(ui->forceButton, SIGNAL(clicked()), this, SLOT(force()));
    connect(ui->genButton, SIGNAL(clicked()), this, SLOT(possion()));
    this->source = nullptr;
    this->source_tmp = nullptr;
    this->background = nullptr;
    this->background_tmp = nullptr;
    this->forceCloneImg = nullptr;
    this->possionCloneImg = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSourceImage() {
    this->source = loadImage();
    if (this->source_tmp) {
        delete this->source_tmp;
        this->source_tmp = new QImage(*(this->source));
    } else {
        this->source_tmp = new QImage(*(this->source));
    }
    update();
}

void MainWindow::loadBackgroundImage() {
    this->background = loadImage();
    if (this->background_tmp) {
        delete this->background_tmp;
        this->background_tmp = new QImage(*(this->background));
    } else {
        this->background_tmp = new QImage(*(this->background));
    }
    update();
}

void MainWindow::clearDraw() {
    if (this->source_tmp) {
        delete this->source_tmp;
        this->source_tmp = new QImage(*(this->source));
    }
    if (this->background_tmp) {
        delete this->background_tmp;
        this->background_tmp = new QImage(*(this->background));
    }
    update();
}

void MainWindow::force() {
    this->forceCloneImg = new QImage(PossionEdit::forceCloning(*source_tmp, *background));
    update();
}

void MainWindow::possion() {
    this->possionCloneImg = new QImage(PossionEdit::possionCloning(*source, *source_tmp, *background, *background_tmp));
    update();
}

QImage* MainWindow::loadImage() {
    QString imgDir = QFileDialog::getOpenFileName(
                this,
                tr("Open Image"),
                "../images",
                tr("Images (*.png *.xpm *.jpg *.tiff *.bmp)"));
    qDebug() << imgDir << endl;
    QImage *img = new QImage(imgDir);
    *img = img->scaledToWidth(300);
    return img;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (this->source)
        painter.drawImage(200, 50, *(this->source_tmp));
    if(this->background)
        painter.drawImage(530, 50, *(this->background_tmp));
    if(this->forceCloneImg)
        painter.drawImage(200, 400, *(this->forceCloneImg));
    if(this->possionCloneImg)
        painter.drawImage(530, 400, *(this->possionCloneImg));
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() && this->source && this->background){
        bool buttonAtSrc =
                event->x() >= 200 && event->x() <= 200 + this->source->width() &&
                event->y() >= 50 && event->y() <= 50 + this->source->height();
        bool buttonAtBkd =
                event->x() >= 530 && event->x() <= 530 + this->background->width() &&
                event->y() >= 50 && event->y() <= 50 + this->background->height();

        if(buttonAtSrc && (event->buttons() & Qt::LeftButton)){
            const int neighbor = 2;
            int x = event->x() - 200, y = event->y() - 50;
            for(int i = x - neighbor;i < x + neighbor;i++)
                for(int j = y - neighbor;j < y + neighbor;j++)
                    if(i >= 0 && i < (*(this->source_tmp)).width() &&
                       j >= 0 && j < (*(this->source_tmp)).height())
                        (*(this->source_tmp)).setPixel(i, j, qRgb(255,0,0));
        }
        if(buttonAtBkd && (event->buttons() & Qt::LeftButton)){
            const int neighbor = 2;
            int x = event->x() - 530, y = event->y() - 50;
            for(int i = x - neighbor;i < x + neighbor;i++)
                for(int j = y - neighbor;j < y + neighbor;j++)
                    if(i >= 0 && i < (*(this->background_tmp)).width() &&
                       j >= 0 && j < (*(this->background_tmp)).height())
                        (*(this->background_tmp)).setPixel(i, j, qRgb(255,0,0));
        }
        if(buttonAtBkd && (event->buttons() & Qt::RightButton)){
            const int neighbor = 2;
            int x = event->x() - 530, y = event->y() - 50;
            for(int i = x - neighbor;i < x + neighbor;i++)
                for(int j = y - neighbor;j < y + neighbor;j++)
                    if(i >= 0 && i < (*(this->background_tmp)).width() &&
                       j >= 0 && j < (*(this->background_tmp)).height())
                        (*(this->background_tmp)).setPixel(i, j, qRgb(128,0,128));
        }
    }
    update();
}
