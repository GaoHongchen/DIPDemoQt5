#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CreateActions();
    CreateMenus();

    InitMainWindow();
}

//创建菜单项
void MainWindow::CreateActions()
{
    actionOpenImg = new QAction(tr("打开图像"),this);
    connect(actionOpenImg,SIGNAL(triggered(bool)),this,SLOT(slotOpenImgSrc()));

    actionSaveImgSrc = new QAction(tr("保存原始图像..."),this);
    connect(actionSaveImgSrc,SIGNAL(triggered(bool)),this,SLOT(slotSaveImgSrc()));

    actionSaveImgDst = new QAction(tr("保存目标图像..."),this);
    connect(actionSaveImgDst,SIGNAL(triggered(bool)),this,SLOT(slotSaveImgDst()));

    actionGray = new QAction(tr("图像灰度化"),this);
    connect(actionGray,SIGNAL(triggered(bool)),this,SLOT(slotGrayImg()));
}

//创建菜单，添加菜单项
void MainWindow::CreateMenus()
{
    menuFile = ui->menuBar->addMenu(tr("文件"));
    menuFile->addAction(actionOpenImg);
    menuFile->addAction(actionSaveImgSrc);
    menuFile->addAction(actionSaveImgDst);

    menuPointOperate = ui->menuBar->addMenu(tr("点运算"));
    menuPointOperate->addAction(actionGray);
}

//初始化主窗口布局
void MainWindow::InitMainWindow()
{
    //原图像和目标图像标题显示区域定义
    labelSrcImgTitle = new QLabel(tr("Source Image Title"));
    labelDstImgTitle = new QLabel(tr("Destination Image Title"));
    labelSrcImgTitle->setAlignment(Qt::AlignCenter);
    labelDstImgTitle->setAlignment(Qt::AlignCenter);
    labelSrcImgTitle->setFrameShape(QFrame::Box);
    labelDstImgTitle->setFrameShape(QFrame::Box);
    //原图像和目标图像显示区域定义
    labelSrcImg = new QLabel(tr("Source Image"));
    labelDstImg = new QLabel(tr("Destination Image"));
    labelSrcImg->setFrameShape(QFrame::Box);
    labelSrcImg->setAlignment(Qt::AlignCenter);
    labelDstImg->setFrameShape(QFrame::Box);
    labelDstImg->setAlignment(Qt::AlignCenter);
    //原图像和目标图像信息显示区域定义
    labelSrcImgInfos = new QLabel(tr("Source Image Infos"));
    labelDstImgInfos = new QLabel(tr("Destination Image Infos"));
    labelSrcImgInfos->setFrameShape(QFrame::Box);
    labelSrcImgInfos->setAlignment(Qt::AlignCenter);
    labelDstImgInfos->setFrameShape(QFrame::Box);
    labelDstImgInfos->setAlignment(Qt::AlignCenter);

    //主窗口控件布局
    layoutGrid = new QGridLayout();
    layoutGrid->addWidget(labelSrcImgTitle,0,0,1,1);
    layoutGrid->addWidget(labelDstImgTitle,0,1,1,1);
    layoutGrid->addWidget(labelSrcImg,1,0,10,1);
    layoutGrid->addWidget(labelDstImg,1,1,10,1);
    layoutGrid->addWidget(labelSrcImgInfos,11,0,1,1);
    layoutGrid->addWidget(labelDstImgInfos,11,1,1,1);
    layoutGrid->setColumnStretch(0,1);
    layoutGrid->setColumnStretch(1,1);

    widgetMain = new QWidget();
    this->setCentralWidget(widgetMain);
    widgetMain->setLayout(layoutGrid);
}

void MainWindow::slotOpenImgSrc()
{
    QFileInfo fileImage;
    int ret = optImgFile.OpenImage(fileImage);
    if(ret == 0)
    {
        nameSrcImg = fileImage.fileName();
        pathSrcImg = fileImage.filePath();
        dirSrcImg  = fileImage.absolutePath();

        //读取并显示源图像
        imgSrc = cv::imread(pathSrcImg.toLocal8Bit().data());
        if(imgSrc.data)
        {
            DisplayImage(imgSrc,
                         labelSrcImgTitle,0,nameSrcImg,dirSrcImg,
                         labelSrcImg,labelSrcImgInfos);
        }
        else
        {
            QMessageBox::critical(this,tr("图像错误"),tr("读取图像失败！"),
                                 QMessageBox::Yes);
            return;
        }
    }
}

void MainWindow::slotSaveImgSrc()
{
    int ret = optImgFile.SaveImage(imgSrc);
    if(ret == 0)
    {
        QMessageBox::information(this,
                                 "保存成功",
                                 "保存原始图像成功",
                                 QMessageBox::Yes);
    }
    else if(ret == -1)
    {
        QMessageBox::critical(this,
                              "图像错误",
                              "原始图像不存在",
                              QMessageBox::Yes);
    }
}

void MainWindow::slotSaveImgDst()
{
    int ret = optImgFile.SaveImage(imgDst);
    if(ret == 0)
    {
        QMessageBox::information(this,
                                 "保存成功",
                                 "保存目标图像成功",
                                 QMessageBox::Yes);
    }
    else if(ret == -1)
    {
        QMessageBox::critical(this,
                              "图像错误",
                              "目标图像不存在",
                              QMessageBox::Yes);
    }
}

void MainWindow::slotGrayImg()
{
    if(imgSrc.empty())
    {
        QMessageBox::critical(this,
                              "图像错误",
                              "原始图像不存在",
                              QMessageBox::Yes);
        return;
    }
    //转换为灰度图
    cv::Mat imgGray;
    imgGray.create(imgSrc.rows,imgSrc.cols,CV_8U);
    cv::cvtColor(imgSrc,imgGray,CV_BGR2GRAY);

    imgDst = imgGray;
    nameDstImg = "Gray_" + nameSrcImg;
    dirDstImg = dirSrcImg;
    DisplayImage(imgDst,
                 labelDstImgTitle,1,nameDstImg,dirDstImg,
                 labelDstImg,labelDstImgInfos);
}

void MainWindow::DisplayImage(cv::Mat matImage,
                              QLabel *labelImageTitle,
                              int SrcOrDst,
                              QString nameImage,
                              QString dirImage,
                              QLabel *labelImage,
                              QLabel *labelImageInfos)
{
    cv::Mat image = matImage;
    int W_LabelImg = labelImage->width();
    int H_LabelImg = labelImage->height();
    int W_Img = image.cols;
    int H_Img = image.rows;
    int N_Channels = image.channels();

    if(SrcOrDst==0)
    {
        labelImageTitle->setText("源图像\r\n名称："
                                 + nameImage
                                 + "\r\n目录："
                                 + dirImage);
    }
    else
    {
        labelImageTitle->setText("目标图像");
    }


    //显示图像信息
    labelImageInfos->setText("宽度："
                        + QString::number(W_Img)
                        + "，高度："
                        + QString::number(H_Img)
                        + "，通道数："
                        + QString::number(N_Channels));

    //根据显示图像的label大小改变图像尺寸
    if(W_Img>W_LabelImg && H_Img<=H_LabelImg)
    {
        cv::resize(image,image,
                   cv::Size(W_LabelImg,H_Img/((float)W_Img/W_LabelImg)));
    }
    else if(W_Img<=W_LabelImg && H_Img>H_LabelImg)
    {
        cv::resize(image,image,
                   cv::Size(W_Img/((float)H_Img/H_LabelImg),H_LabelImg));
    }
    else if(W_Img>W_LabelImg && H_Img>H_LabelImg)
    {
        if(W_LabelImg>=H_LabelImg)
        {
            if(W_Img<=H_Img)
            {
                cv::resize(image,image,
                           cv::Size(W_Img/((float)H_Img/H_LabelImg),H_LabelImg));
            }
            else
            {
                if(W_Img/H_Img >= W_LabelImg/H_LabelImg)
                {
                    cv::resize(image,image,
                               cv::Size(W_LabelImg,H_Img/((float)W_Img/W_LabelImg)));
                }
                else
                {
                    cv::resize(image,image,
                               cv::Size(W_Img/((float)H_Img/H_LabelImg),H_LabelImg));
                }
            }
        }
        else
        {
            if(W_Img>=H_Img)
            {
                cv::resize(image,image,
                           cv::Size(W_LabelImg,H_Img/((float)W_Img/W_LabelImg)));
            }
            else
            {
                if(H_Img/W_Img <= H_LabelImg/W_LabelImg)
                {
                    cv::resize(image,image,
                               cv::Size(W_LabelImg,H_Img/((float)W_Img/W_LabelImg)));
                }
                else
                {
                    cv::resize(image,image,
                               cv::Size(W_Img/((float)H_Img/H_LabelImg),H_LabelImg));
                }
            }
        }
    }
    QImage qImg;
    if(image.type() == CV_8UC3)
    {
        cv::cvtColor(image,image,CV_BGR2RGB);
        qImg = QImage((const unsigned char*)(image.data),
                      image.cols,image.rows,
                      image.cols*image.channels(),
                      QImage::Format_RGB888);
    }
    if(image.type() == CV_8UC1)// 8-bits unsigned, NO. OF CHANNELS = 1
    {
        qImg = QImage(image.cols,
                      image.rows,
                      QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        qImg.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            qImg.setColor(i, qRgb(i, i, i));
        }
        // Copy input image
        uchar *pSrc = image.data;
        for(int row = 0; row < image.rows; row ++)
        {
            uchar *pDest = qImg.scanLine(row);
            memcpy(pDest, pSrc, image.cols);
            pSrc += image.step;
        }
    }
    labelImage->setPixmap(QPixmap::fromImage(qImg));
    //labelImage->resize(labelImage->pixmap()->size());
}

void MainWindow::resizeEvent(QResizeEvent *)
{
}

MainWindow::~MainWindow()
{
    delete ui;

    delete menuFile;
    delete actionOpenImg;
    delete actionSaveImgSrc;
    delete actionSaveImgDst;

    delete menuPointOperate;
    delete actionGray;

    delete labelSrcImg;
    delete labelDstImg;
    delete labelSrcImgInfos;
    delete labelDstImgInfos;

    delete layoutGrid;
    delete widgetMain;
}
