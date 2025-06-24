#pragma execution_character_set("utf-8")

#include "handlerectitem.h"

#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>

#include "./canvasrectitem.h"

#include "../qtimageviewer.h"


HandleRectItem::HandleRectItem(const QRectF &rect, Qtimageviewer *viewer)
    : QGraphicsRectItem(rect) {
    setAcceptHoverEvents(true);
    this->viewer = viewer;
}

void HandleRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    updateCursor(event->pos());
    QGraphicsRectItem::hoverMoveEvent(event);
}

void HandleRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    handle = detectHandle(event->pos());
    if (handle != None) {
        this->viewer->unselectAll();
        setSelected(true);

        origRect = rect();
        origMousePos = event->pos();
    } else {
        QGraphicsRectItem::mousePressEvent(event);
    }
}

void HandleRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (handle != None) {
        QPointF delta = event->pos() - origMousePos;
        QRectF r = origRect;

        switch (handle) {
        case TopLeft:
            r.setTopLeft(r.topLeft() + delta);
            break;
        case Top:
            r.setTop(r.top() + delta.y());
            break;
        case TopRight:
            r.setTopRight(r.topRight() + delta);
            break;
        case Right:
            r.setRight(r.right() + delta.x());
            break;
        case BottomRight:
            r.setBottomRight(r.bottomRight() + delta);
            break;
        case Bottom:
            r.setBottom(r.bottom() + delta.y());
            break;
        case BottomLeft:
            r.setBottomLeft(r.bottomLeft() + delta);
            break;
        case Left:
            r.setLeft(r.left() + delta.x());
            break;
        default:
            break;
        }
        // 限制最小尺寸
        if (r.width() < 50)
            r.setWidth(50);
        if (r.height() < 50)
            r.setHeight(50);
        setRect(r.normalized());
        viewer->canvas->setRect(this->rect().adjusted(8, 8, -8, -8));
    } else {
        QGraphicsRectItem::mouseMoveEvent(event);
    }
}

void HandleRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    handle = None;
    QGraphicsRectItem::mouseReleaseEvent(event);
}

void HandleRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->save();
    // painter->setClipRect(boundingRect());
    painter->setBrush(Qt::transparent);
    // QGraphicsRectItem::paint(painter, option, widget);

    if (viewer->mode == Qtimageviewer::Edit) {
        const int size = 8; // 画8个缩放手柄
        painter->setBrush(Qt::white);
        painter->setPen(QPen(Qt::blue, 1));
        for (const QPointF &p : handlePoints()) {
            painter->drawRect(QRectF(p.x(), p.y(), size, size));
        }

        setPen(QPen(Qt::blue, 2, Qt::DashLine)); // 外框虚线
        painter->setBrush(Qt::transparent);
        painter->drawRect(option->rect.adjusted(4, 4, -4, -4));
    }

    painter->restore();
}

QPainterPath HandleRectItem::shape() const {
    QPainterPath path;
    QRectF r = rect();
    const qreal handleSize = 8;

    // Top
    path.addRect(QRectF(r.left(), r.top(), r.width(), handleSize));
    // Bottom
    path.addRect(QRectF(r.left(), r.bottom() - handleSize, r.width(), handleSize));
    // Left
    path.addRect(QRectF(r.left(), r.top() + handleSize, handleSize, r.height() - 2 * handleSize));
    // Right
    path.addRect(QRectF(r.right() - handleSize, r.top() + handleSize, handleSize, r.height() - 2 * handleSize));

    return path;
}


void HandleRectItem::updateCursor(const QPointF &pos) {
    Handle h = detectHandle(pos);
    switch (h) {
    case TopLeft:
    case BottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case TopRight:
    case BottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case Top:
    case Bottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case Left:
    case Right:
        setCursor(Qt::SizeHorCursor);
        break;
    default:
        setCursor(Qt::ArrowCursor);
        break;
    }
}

HandleRectItem::Handle HandleRectItem::detectHandle(const QPointF &pos) const {
    const int range = 8;
    for (int i = 0; i < handlePoints().size(); ++i) {
        if (QRectF(handlePoints()[i].x(), handlePoints()[i].y(), range, range).contains(pos)) {
            return static_cast<Handle>(i + 1);
        }
    }
    return None;
}

QList<QPointF> HandleRectItem::handlePoints() const {
    QRectF r = rect();
    return {
        r.topLeft(),                             // TopLeft
        QPointF(r.center().x(), r.top()),        // Top
        QPointF(r.right() - 8, r.top()),         // TopRight
        QPointF(r.right() - 8, r.center().y()),  // Right
        QPointF(r.right() - 8, r.bottom() - 8),  // BottomRight
        QPointF(r.center().x(), r.bottom() - 8), // Bottom
        QPointF(r.left(), r.bottom() - 8),       // BottomLeft
        QPointF(r.left(), r.center().y())        // Left
    };
}
