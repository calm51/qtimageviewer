#pragma execution_character_set("utf-8")

#include "canvasrectitem.h"

#include <QCursor>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QPen>

#include "../qtimageviewer.h"


CanvasRectItem::CanvasRectItem(const QRectF &rect, Qtimageviewer *viewer)
    : QGraphicsRectItem(rect) {
    setPen(Qt::NoPen);
    this->viewer = viewer;
}

void CanvasRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->save();
    QGraphicsRectItem::paint(painter, option, widget);
    painter->restore();
}
