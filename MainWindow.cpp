
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Icon.h"
#include <QPen>
#include <QPixmap>
#include <QPainter>
#include <QPushButton>
#include <QDebug>
#include <QPainterPath>
#include <QCoreApplication>
#include "ScreenShoter.h"



MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);   //todo | Qt::WindowStaysOnTopHint
    this->setCursor(Qt::CrossCursor);
    this->setMouseTracking(true);
    qApp->installEventFilter(this);
    initMask();
    initDragger();
    initCanvasImg();
    initToolMain();
    initToolRect();
    initToolEraser();
    this->showMaximized(); //todo
}

MainWindow::~MainWindow()
{
    delete painter1;
    delete painter2;
    delete canvasImg1;
    delete canvasImg2;
    delete ui;
}

void MainWindow::initCanvasImg()
{
    auto scaleFactor = metric(PdmDevicePixelRatioScaled) / devicePixelRatioFScale();
    canvasImg1 = new QImage(ScreenShoter::Get()->screenRects[0].size()*scaleFactor, QImage::Format_ARGB32_Premultiplied);
    canvasImg1->setDevicePixelRatio(scaleFactor);
    canvasImg1->fill(0);
    canvasImg2 = new QImage(ScreenShoter::Get()->screenRects[0].size()*scaleFactor, QImage::Format_ARGB32_Premultiplied);
    canvasImg2->setDevicePixelRatio(scaleFactor);
    canvasImg2->fill(0);
    painter1 = new QPainter(canvasImg1);
    //    painter1->setRenderHint(QPainter::Antialiasing, true);
    //    painter1->setRenderHint(QPainter::SmoothPixmapTransform, false);

    painter2 = new QPainter(canvasImg2);
    //    painter2->setRenderHint(QPainter::Antialiasing, true);
    //    painter2->setRenderHint(QPainter::SmoothPixmapTransform, false);
}


void MainWindow::initMask()
{
    auto screenShoter = ScreenShoter::Get();
    maskPath.moveTo(0 - maskBorderWidth, 0 - maskBorderWidth);
    maskPath.lineTo(screenShoter->screenRects[0].width() + maskBorderWidth, 0 - maskBorderWidth);
    maskPath.lineTo(screenShoter->screenRects[0].width() + maskBorderWidth, screenShoter->screenRects[0].height() + maskBorderWidth);
    maskPath.lineTo(0 - maskBorderWidth, screenShoter->screenRects[0].height() + maskBorderWidth);
    maskPath.lineTo(0 - maskBorderWidth, 0 - maskBorderWidth);

    maskPath.moveTo(0 - maskBorderWidth, 0 - maskBorderWidth);
    maskPath.lineTo(screenShoter->screenRects[0].width() + maskBorderWidth, 0 - maskBorderWidth);
    maskPath.lineTo(screenShoter->screenRects[0].width() + maskBorderWidth, screenShoter->screenRects[0].height() + maskBorderWidth);
    maskPath.lineTo(0 - maskBorderWidth, screenShoter->screenRects[0].height() + maskBorderWidth);
    maskPath.lineTo(0 - maskBorderWidth, 0 - maskBorderWidth);
}

void MainWindow::initDragger()
{
    for (int var = 0; var < 8; ++var)
    {
        dragers.append(QRectF(0, 0, 8, 8));
    }
}


void MainWindow::draggerPressed(int draggerIndex)
{
    state = "lastPathDrag";
    isMouseDown = true;
    this->draggerIndex = draggerIndex;
    mousePressPoint = QCursor::pos();
}



void MainWindow::resizeMask(const QPointF& point)
{
    if (resizeMaskIndex == 0)
    {
        maskPath.setElementPositionAt(5, point.x(), point.y());
        maskPath.setElementPositionAt(6, maskPath.elementAt(6).x, point.y());
        maskPath.setElementPositionAt(9, point.x(), point.y());
        maskPath.setElementPositionAt(8, point.x(), maskPath.elementAt(8).y);
        repaint();
    }
    else if (resizeMaskIndex == 1)
    {
        maskPath.setElementPositionAt(5, maskPath.elementAt(5).x, point.y());
        maskPath.setElementPositionAt(6, maskPath.elementAt(6).x, point.y());
        maskPath.setElementPositionAt(9, maskPath.elementAt(9).x, point.y());
        repaint();
    }
    else if (resizeMaskIndex == 2)
    {
        maskPath.setElementPositionAt(5, maskPath.elementAt(5).x, point.y());
        maskPath.setElementPositionAt(6, point.x(), point.y());
        maskPath.setElementPositionAt(7, point.x(), maskPath.elementAt(7).y);
        maskPath.setElementPositionAt(9, maskPath.elementAt(9).x, point.y());
        repaint();
    }
    else if (resizeMaskIndex == 3)
    {
        maskPath.setElementPositionAt(6, point.x(), maskPath.elementAt(6).y);
        maskPath.setElementPositionAt(7, point.x(), maskPath.elementAt(7).y);
        repaint();
    }
    else if (resizeMaskIndex == 4)
    {
        maskPath.setElementPositionAt(6, point.x(), maskPath.elementAt(6).y);
        maskPath.setElementPositionAt(7, point.x(), point.y());
        maskPath.setElementPositionAt(8, maskPath.elementAt(8).x, point.y());
        repaint();
    }
    else if (resizeMaskIndex == 5)
    {
        maskPath.setElementPositionAt(7, maskPath.elementAt(7).x, point.y());
        maskPath.setElementPositionAt(8, maskPath.elementAt(8).x, point.y());
        repaint();
    }
    else if (resizeMaskIndex == 6)
    {
        maskPath.setElementPositionAt(7, maskPath.elementAt(7).x, point.y());
        maskPath.setElementPositionAt(8, point.x(), point.y());
        maskPath.setElementPositionAt(9, point.x(), maskPath.elementAt(9).y);
        maskPath.setElementPositionAt(5, point.x(), maskPath.elementAt(5).y);
        repaint();
    }
    else if (resizeMaskIndex == 7)
    {
        maskPath.setElementPositionAt(8, point.x(), maskPath.elementAt(8).y);
        maskPath.setElementPositionAt(9, point.x(), maskPath.elementAt(9).y);
        maskPath.setElementPositionAt(5, point.x(), maskPath.elementAt(5).y);
        repaint();
    }
    else if (resizeMaskIndex == 8)
    {
        if (point == mousePressPoint)
        {
            return;
        }
        else
        {
            qreal xSpan = point.x() - mousePressPoint.x();
            qreal ySpan = point.y() - mousePressPoint.y();
            maskPath.setElementPositionAt(5, maskPath.elementAt(5).x + xSpan, maskPath.elementAt(5).y + ySpan);
            maskPath.setElementPositionAt(6, maskPath.elementAt(6).x + xSpan, maskPath.elementAt(6).y + ySpan);
            maskPath.setElementPositionAt(7, maskPath.elementAt(7).x + xSpan, maskPath.elementAt(7).y + ySpan);
            maskPath.setElementPositionAt(8, maskPath.elementAt(8).x + xSpan, maskPath.elementAt(8).y + ySpan);
            maskPath.setElementPositionAt(9, maskPath.elementAt(9).x + xSpan, maskPath.elementAt(9).y + ySpan);
            mousePressPoint = point;
            repaint();
        }
    }
}

void MainWindow::resizePath(const QPointF& point)
{
    auto& path = paths.last();
    if (draggerIndex == 0)
    {
        path.setElementPositionAt(0, point.x(), point.y());
        path.setElementPositionAt(1, path.elementAt(1).x, point.y());
        path.setElementPositionAt(3, point.x(), path.elementAt(3).y);
        path.setElementPositionAt(4, point.x(), point.y());
    }
    else if (draggerIndex == 1)
    {
        path.setElementPositionAt(0, path.elementAt(0).x, point.y());
        path.setElementPositionAt(1, path.elementAt(1).x, point.y());
        path.setElementPositionAt(4, path.elementAt(4).x, point.y());
    }
    else if (draggerIndex == 2)
    {
        path.setElementPositionAt(0, path.elementAt(0).x, point.y());
        path.setElementPositionAt(1, point.x(), point.y());
        path.setElementPositionAt(2, point.x(), path.elementAt(2).y);
        path.setElementPositionAt(4, path.elementAt(4).x, point.y());
    }
    else if (draggerIndex == 3)
    {
        path.setElementPositionAt(1, point.x(), path.elementAt(1).y);
        path.setElementPositionAt(2, point.x(), path.elementAt(2).y);
    }
    else if (draggerIndex == 4)
    {
        path.setElementPositionAt(1, point.x(), path.elementAt(1).y);
        path.setElementPositionAt(2, point.x(), point.y());
        path.setElementPositionAt(3, path.elementAt(3).x, point.y());
    }
    else if (draggerIndex == 5)
    {
        path.setElementPositionAt(2, path.elementAt(2).x, point.y());
        path.setElementPositionAt(3, path.elementAt(3).x, point.y());
    }
    else if (draggerIndex == 6)
    {
        path.setElementPositionAt(2, path.elementAt(2).x, point.y());
        path.setElementPositionAt(3, point.x(), point.y());
        path.setElementPositionAt(4, point.x(), path.elementAt(4).y);
        path.setElementPositionAt(0, point.x(), path.elementAt(0).y);
    }
    else if (draggerIndex == 7)
    {
        path.setElementPositionAt(3, point.x(), path.elementAt(3).y);
        path.setElementPositionAt(4, point.x(), path.elementAt(4).y);
        path.setElementPositionAt(0, point.x(), path.elementAt(0).y);
    }
    else if (draggerIndex == 8)
    {
        if (point == mousePressPoint)
        {
            return;
        }
        else
        {
            qreal xSpan = point.x() - mousePressPoint.x();
            qreal ySpan = point.y() - mousePressPoint.y();
            maskPath.setElementPositionAt(5, maskPath.elementAt(5).x + xSpan, maskPath.elementAt(5).y + ySpan);
            maskPath.setElementPositionAt(6, maskPath.elementAt(6).x + xSpan, maskPath.elementAt(6).y + ySpan);
            maskPath.setElementPositionAt(7, maskPath.elementAt(7).x + xSpan, maskPath.elementAt(7).y + ySpan);
            maskPath.setElementPositionAt(8, maskPath.elementAt(8).x + xSpan, maskPath.elementAt(8).y + ySpan);
            maskPath.setElementPositionAt(9, maskPath.elementAt(9).x + xSpan, maskPath.elementAt(9).y + ySpan);
            mousePressPoint = point;
        }
    }
    showDragger(path.elementAt(0).x, path.elementAt(0).y, path.elementAt(2).x, path.elementAt(2).y);
    memcpy(canvasImg1->bits(), canvasImg2->bits(), canvasImg1->sizeInBytes());
    painter1->setPen(QPen(QBrush(Qt::red), 2));
    painter1->setBrush(Qt::NoBrush);
    painter1->drawPath(path);
    repaint();
}

bool MainWindow::isMouseInDragger(const QPointF& point)
{
    for (int var = 0; var < showDraggerCount; ++var)
    {
        if (dragers[var].contains(point))
        {
            draggerIndex = var;
            return true;
        }
    }
    return false;
}

int MainWindow::pointInMaskArea(const QPointF& curPoint)
{
    QRectF rect(QPointF(maskPath.elementAt(5).x, maskPath.elementAt(5).y), QPointF(maskPath.elementAt(7).x, maskPath.elementAt(7).y));
    if (rect.contains(curPoint))
    {
        return 8;
    }
    else
    {
        if (curPoint.x() < rect.left() && curPoint.y() < rect.top())
        {
            return 0;
        }
        else if (curPoint.x() > rect.left() && curPoint.y() < rect.top() && curPoint.x() < rect.right())
        {
            return 1;
        }
        else if (curPoint.x() > rect.right() && curPoint.y() < rect.top())
        {
            return 2;
        }
        else if (curPoint.x() > rect.right() && curPoint.y() > rect.top() && curPoint.y() < rect.bottom())
        {
            return 3;
        }
        else if (curPoint.x() > rect.right()  && curPoint.y() > rect.bottom())
        {
            return 4;
        }
        else if (curPoint.x() > rect.left() && curPoint.x() < rect.right() && curPoint.y() > rect.bottom())
        {
            return 5;
        }
        else if (curPoint.x() < rect.left()  && curPoint.y() > rect.bottom())
        {
            return 6;
        }
        else if (curPoint.x() < rect.left()  && curPoint.y() > rect.top() && curPoint.y() < rect.bottom())
        {
            return 7;
        }
    }

}

void MainWindow::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
//    p.setRenderHint(QPainter::Antialiasing, true);
//    p.setRenderHint(QPainter::SmoothPixmapTransform, false);
    p.drawPixmap(0, 0, ScreenShoter::Get()->desktopImages[0]);
    p.drawImage(0, 0, *canvasImg1);

    if (showDraggerCount > 0)
    {
        QPainterPath draggerPath;
        for (int var = 0; var < showDraggerCount; ++var)
        {
            draggerPath.addRect(dragers[var]);
        }
        p.setPen(QPen(QBrush(QColor(0, 0, 0)), 1));
        p.setBrush(Qt::NoBrush);
        p.drawPath(draggerPath);
    }


    p.setPen(QPen(QBrush(QColor(22, 119, 255)), maskBorderWidth));
    p.setBrush(QBrush(QColor(0, 0, 0, 120)));
    p.drawPath(maskPath);
    p.end();
}

void MainWindow::DoneRectEllipse()
{
    auto& path = paths.last();
    qreal x2 = -999.0, x1 = 999999999.0;
    qreal y2 = -999.0, y1 = 999999999.0;
    for (int var = 0; var < 5; ++var)
    {
        auto ele = path.elementAt(var);
        if (ele.x < x1)
        {
            x1 = ele.x;
        }
        if (ele.x > x2)
        {
            x2 = ele.x;
        }
        if (ele.y < y1)
        {
            y1 = ele.y;
        }
        if (ele.y > y2)
        {
            y2 = ele.y;
        }
    }
    path.setElementPositionAt(0, x1, y1);
    path.setElementPositionAt(1, x2, y1);
    path.setElementPositionAt(2, x2, y2);
    path.setElementPositionAt(3, x1, y2);
    path.setElementPositionAt(4, x1, y1);
    //            painter2->setPen(QPen(QBrush(Qt::red), 2));
    //            painter2->setBrush(Qt::NoBrush);
    //            painter2->drawPath(path);
    ui->btnUndo->setStyleSheet("");
    showDragger(x1, y1, x2, y2);
    setCursor(Qt::CrossCursor);
}

void MainWindow::showDragger(qreal x1, qreal y1, qreal x2, qreal y2)
{
    dragers[0].moveTo(x1 - 4, y1 - 4);
    dragers[1].moveTo(x1 + (x2 - x1) / 2 - 4, y1 - 4);
    dragers[2].moveTo(x2 - 4, y1 - 4);
    dragers[3].moveTo(x2 - 4, y1 + (y2 - y1) / 2 - 4);
    dragers[4].moveTo(x2 - 4, y2 - 4);
    dragers[5].moveTo(x1 + (x2 - x1) / 2 - 4, y2 - 4);
    dragers[6].moveTo(x1 - 4, y2 - 4);
    dragers[7].moveTo(x1 - 4, y1 + (y2 - y1) / 2 - 4);
    showDraggerCount = 8;
}

void MainWindow::initToolMain()
{
    ui->btnRectEllipse->setFont(Icon::font);
    ui->btnRectEllipse->setText(Icon::icons[Icon::Name::rectEllipse]);
    QObject::connect(ui->btnRectEllipse,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnArrow->setFont(Icon::font);
    ui->btnArrow->setText(Icon::icons[Icon::Name::arrow]);
    connect(ui->btnArrow,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnLine->setFont(Icon::font);
    ui->btnLine->setText(Icon::icons[Icon::Name::line]);
    QObject::connect(ui->btnLine,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnMosaic->setFont(Icon::font);
    ui->btnMosaic->setText(Icon::icons[Icon::Name::mosaic]);
    QObject::connect(ui->btnMosaic,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnUndo->setFont(Icon::font);
    ui->btnUndo->setText(Icon::icons[Icon::Name::undo]);
    QObject::connect(ui->btnUndo,  &QPushButton::clicked, this, &MainWindow::undo);

    ui->btnRedo->setFont(Icon::font);
    ui->btnRedo->setText(Icon::icons[Icon::Name::redo]);
    QObject::connect(ui->btnRedo,  &QPushButton::clicked, this, &MainWindow::redo);

    ui->btnOk->setFont(Icon::font);
    ui->btnOk->setText(Icon::icons[Icon::Name::ok]);
    QObject::connect(ui->btnOk,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnSave->setFont(Icon::font);
    ui->btnSave->setText(Icon::icons[Icon::Name::save]);
    QObject::connect(ui->btnSave,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnCopy->setFont(Icon::font);
    ui->btnCopy->setText(Icon::icons[Icon::Name::copy]);
    QObject::connect(ui->btnCopy,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnText->setFont(Icon::font);
    ui->btnText->setText(Icon::icons[Icon::Name::text]);
    QObject::connect(ui->btnText,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnEraser->setFont(Icon::font);
    ui->btnEraser->setText(Icon::icons[Icon::Name::eraser]);
    QObject::connect(ui->btnEraser,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnNumber->setFont(Icon::font);
    ui->btnNumber->setText(Icon::icons[Icon::Name::number]);
    QObject::connect(ui->btnNumber,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolMain->hide();
    ui->toolMain->setStyleSheet(style.arg("toolMain"));

}

void MainWindow::initToolEraser()
{
    ui->btnEraserDot->setFont(Icon::font);
    ui->btnEraserDot->setText(Icon::icons[Icon::Name::dot]);
    QObject::connect(ui->btnEraserDot,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnEraserRect->setFont(Icon::font);
    ui->btnEraserRect->setText(Icon::icons[Icon::Name::rectFill]);
    QObject::connect(ui->btnEraserRect,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolEraser->hide();
    ui->toolEraser->setStyleSheet(style.arg("toolEraser"));
}

void MainWindow::initToolRect()
{
    ui->btnDot->setFont(Icon::font);
    ui->btnDot->setText(Icon::icons[Icon::Name::dot]);
    QObject::connect(ui->btnDot,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnFill->setFont(Icon::font);
    ui->btnFill->setText(Icon::icons[Icon::Name::rectFill]);
    QObject::connect(ui->btnEllipse,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnRect->setFont(Icon::font);
    ui->btnRect->setText(Icon::icons[Icon::Name::rect]);
    QObject::connect(ui->btnRect,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->btnEllipse->setFont(Icon::font);
    ui->btnEllipse->setText(Icon::icons[Icon::Name::ellipse]);
    QObject::connect(ui->btnEllipse,  &QPushButton::clicked, this, &MainWindow::btnMainToolSelected);

    ui->toolRectEllipse->hide();
    ui->toolRectEllipse->setStyleSheet(style.arg("toolRectEllipse"));
}

void MainWindow::switchTool(const QString& toolName)
{
    for (auto item : this->centralWidget()->children())
    {
        auto name = item->objectName();
        auto tool = qobject_cast<QWidget*>(item);
        if (!tool || name == "toolMain")
        {
            continue;
        }
        if (name == toolName)
        {
            state = name.remove("tool");
            setCursor(Qt::CrossCursor);
            tool->move(ui->toolMain->x(), ui->toolMain->y() + ui->toolMain->height() + 4);
            tool->show();
        }
        else
        {
            tool->hide();
        }
    }
}

void MainWindow::undo()
{
    if (paths.count() < 1)
    {
        return;
    }
    if (undoIndex == 0)
    {
        return;
    }
    else if (undoIndex == -1)
    {
        undoIndex = paths.count() - 1;
    }
    else
    {
        undoIndex -= 1;
    }
    canvasImg2->fill(0);
    for (int var = 0; var < paths.count(); ++var)
    {
        if (var >= undoIndex)
        {
            break;
        }
        painter2->setPen(QPen(QBrush(Qt::red), 2));
        painter2->setBrush(Qt::NoBrush);
        painter2->drawPath(paths[var]);
    }
    memcpy(canvasImg1->bits(), canvasImg2->bits(), canvasImg1->sizeInBytes());
    repaint();
    ui->btnRedo->setStyleSheet("");
    if (undoIndex == 0)
    {
        ui->btnUndo->setStyleSheet("color:#999;");
    }
    else
    {
        ui->btnUndo->setStyleSheet("");
    }

}
void MainWindow::redo()
{
    if (undoIndex == -1)
    {
        return;
    }
    undoIndex += 1;
    canvasImg2->fill(0);
    for (int var = 0; var < paths.count(); ++var)
    {
        if (var >= undoIndex)
        {
            break;
        }
        painter2->setPen(QPen(QBrush(Qt::red), 2));
        painter2->setBrush(Qt::NoBrush);
        painter2->drawPath(paths[var]);
    }
    memcpy(canvasImg1->bits(), canvasImg2->bits(), canvasImg1->sizeInBytes());
    repaint();
    if (undoIndex >= paths.count())
    {
        ui->btnRedo->setStyleSheet("color:#999;");
        undoIndex = -1;
    }
}

void MainWindow::btnMainToolSelected()
{
    for (auto item : ui->mainToolMenuContainer->children())
    {
        auto name = item->objectName();
        auto btn = qobject_cast<QPushButton*>(item);
        if (!btn)
        {
            continue;
        }
        if (btn->isChecked())
        {
            switchTool(name.replace("btn", "tool"));
            break;
        }
    }
}

void MainWindow::showToolMain()
{
    auto ele0 = maskPath.elementAt(5);
    auto ele1 = maskPath.elementAt(6);
    auto ele2 = maskPath.elementAt(7);
    if (this->height() - ele2.y > 80)
    {
        auto x = ele2.x - ui->toolMain->width();
        if (x > 0)
        {
            ui->toolMain->move(x, ele2.y + 6);
        }
        else
        {
            ui->toolMain->move(ele0.x, ele2.y + 6);
        }
        ui->toolMain->show();
        return;
    }
    if (ele1.y > 80)
    {
        auto x = ele1.x - ui->toolMain->width();
        if (x > 0)
        {
            ui->toolMain->move(x, ele1.y - 6 - 32);
        }
        else
        {
            ui->toolMain->move(ele0.x, ele1.y - 6 - 32);
        }
        ui->toolMain->show();
        return;
    }
    auto x = ele1.x - ui->toolMain->width() - 6;
    if (x > 0)
    {
        ui->toolMain->move(x, ele1.y + 6);
    }
    else
    {
        ui->toolMain->move(ele0.x + 6, ele1.y + 6);
    }
    ui->toolMain->show();
}

