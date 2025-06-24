#pragma execution_character_set("utf-8")

#include "clippedpixmapitem.h"

#include <QPainter>


ClippedPixmapItem::ClippedPixmapItem(const QPixmap &pixmap, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent) {}

void ClippedPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (pixmap().isNull())
        return;
    painter->save();
    if (parentItem()) { // 裁切
        QRectF parentRect = parentItem()->boundingRect();
        QPolygonF clipPolygon = mapFromItem(parentItem(), parentRect);
        QPainterPath clipPath;
        clipPath.addPolygon(clipPolygon);
        painter->setClipPath(clipPath);
    }
    QGraphicsPixmapItem::paint(painter, option, widget);
    painter->restore();
}
