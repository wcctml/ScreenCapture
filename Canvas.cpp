
#include "Canvas.h"
#include "ScreenShoter.h"
#include <QPainterPath>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QApplication>
#include <QColor>
#include <QDebug>
#include "MainWindow.h"

Canvas::Canvas(QObject* parent)
    : QGraphicsScene{parent}
{
    auto screenShoter = ScreenShoter::Get();
    auto view = static_cast<QGraphicsView*>(parent);
    view->setGeometry(screenShoter->screenRects[0]);
    setSceneRect(screenShoter->screenRects[0]);
    //todo 多屏支持
    auto imgItem = addPixmap(screenShoter->desktopImages[0]);
    imgItem->setPos(0, 0);
    initMask();

}
Canvas::~Canvas()
{

}
void Canvas::initMask()
{
    auto screenShoter = ScreenShoter::Get();
    QPainterPath maskPath;
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

    QPen pen(QBrush(QColor(22, 119, 255)), maskBorderWidth);
    maskPathItem = addPath(maskPath, pen, QBrush(QColor(0, 0, 0, 120)));


}
void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (isMouseDown)
    {
        auto curPoint = mouseEvent->scenePos();
        auto path = maskPathItem->path();
        path.setElementPositionAt(5, mousePressPoint.x(), mousePressPoint.y());
        path.setElementPositionAt(6, curPoint.x(), mousePressPoint.y());
        path.setElementPositionAt(7, curPoint.x(), curPoint.y());
        path.setElementPositionAt(8, mousePressPoint.x(), curPoint.y());
        path.setElementPositionAt(9, mousePressPoint.x(), mousePressPoint.y());
        maskPathItem->setPath(path);

    }
}
void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton)
    {
        qApp->exit();
        return;
    }
    else if (mouseEvent->button() == Qt::LeftButton)
    {
        mousePressPoint = mouseEvent->scenePos();
        isMouseDown = true;
    }

}
void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton)
    {
        isMouseDown = false;
        auto mainWin = (MainWindow*)(qApp->activeWindow());
        auto path = maskPathItem->path();
        mainWin->showToolMain(path.elementAt(7).x, path.elementAt(7).y);

    }
}

