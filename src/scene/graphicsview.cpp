#pragma execution_character_set("utf-8")

#include "graphicsview.h"

#include <QMouseEvent>

#include "../qtimageviewer.h"
#include "../utils.h"
#include "./canvasrectitem.h"
#include "./checkerboardscene.h"

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , drawing(false)
    , drawingItem(nullptr) {
    setMouseTracking(true);
}

GraphicsView::~GraphicsView() {
    // if (hLine) {
    //     delete hLine;
    //     hLine = nullptr;
    // }
    // if (vLine) {
    //     delete vLine;
    //     vLine = nullptr;
    // }
}

void GraphicsView::startDraw(QGraphicsRectItem *drawingItem) {
    this->drawingItem = drawingItem;
    drawing = false;

    QPoint viewCenter = viewport()->rect().center();
    // 转换为场景坐标
    QPointF sceneCenter = mapToScene(viewCenter);
    // 获取当前视口对应的 scene 范围
    QRectF sceneRect = mapToScene(viewport()->rect()).boundingRect();
    // 更新十字线
    hLine->setLine(sceneRect.left(), sceneCenter.y(), sceneRect.right(), sceneCenter.y());
    vLine->setLine(sceneCenter.x(), sceneRect.top(), sceneCenter.x(), sceneRect.bottom());

    hLine->setVisible(true);
    vLine->setVisible(true);
}

void GraphicsView::endDraw() {
    hLine->setVisible(false);
    vLine->setVisible(false);
    if (drawingItem) {
        viewer()->scene->removeElement(drawingItem);
        drawingItem = nullptr;
        drawing = false;
    }
}

Qtimageviewer *GraphicsView::viewer() {
    return findParentOfType<Qtimageviewer>(this);
}

void GraphicsView::_new_line() {
    hLine = new QGraphicsLineItem();
    vLine = new QGraphicsLineItem();

    hLine->setVisible(false);
    vLine->setVisible(false);

    hLine->setAcceptedMouseButtons(Qt::NoButton);
    vLine->setAcceptedMouseButtons(Qt::NoButton);

    QPen pen(Qt::red, 1, Qt::DashLine);
    hLine->setPen(pen);
    vLine->setPen(pen);

    hLine->setZValue(99);
    vLine->setZValue(99);

    scene()->addItem(hLine);
    scene()->addItem(vLine);
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        startPoint = mapToScene(event->pos());
        QPointF localPoint = viewer()->canvas->mapFromScene(startPoint);
        if (drawingItem) {
            drawingItem->setAcceptedMouseButtons(Qt::NoButton);
            drawingItem->setPos(localPoint);
            drawingItem->setVisible(true);
            drawing = true;
            event->accept();
            return;
        }
    } else if (event->button() == Qt::RightButton) {
        endDraw();
    }
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {
    if (drawing && drawingItem) {
        QPointF currentPoint = mapToScene(event->pos());
        QPointF localCurrent = viewer()->canvas->mapFromScene(currentPoint);

        QPointF delta = localCurrent - drawingItem->pos();
        QRectF rect(QPointF(0, 0), delta);

        drawingItem->setRect(rect.normalized());
    }

    if (drawingItem) {
        QPoint viewPos = event->pos();
        QPointF scenePos = mapToScene(viewPos);

        // 获取视口的边界并转换到 scene 坐标
        QRect viewRect = viewport()->rect();
        QRectF sceneRect = mapToScene(viewRect).boundingRect();

        // 设置十字线的位置
        hLine->setLine(sceneRect.left(), scenePos.y(), sceneRect.right(), scenePos.y());
        vLine->setLine(scenePos.x(), sceneRect.top(), scenePos.x(), sceneRect.bottom());
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    if (drawing && drawingItem && event->button() == Qt::LeftButton) {
        drawing = false;
        // 如果需要，添加回调或处理完成的矩形
        drawingItem->setAcceptedMouseButtons(Qt::LeftButton);
        viewer()->unselectAll();
        drawingItem->setSelected(true);
        drawingItem = nullptr;

        hLine->setVisible(false);
        vLine->setVisible(false);
    }
    QGraphicsView::mouseReleaseEvent(event);
}
