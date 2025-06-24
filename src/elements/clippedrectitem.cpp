#pragma execution_character_set("utf-8")

#include "clippedrectitem.h"

#include <qaction.h>
#include "../qtimageviewer.h"
#include "../scene/checkerboardscene.h"
#include "../scene/graphicsview.h"
#include <QCursor>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTextCursor>
#include <QTextDocument>


ClippedRectItem::ClippedRectItem(const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent) {
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    setCursor(Qt::SizeAllCursor);
    setAcceptHoverEvents(true);
}

Qtimageviewer *ClippedRectItem::viewer() {
    return qobject_cast<CheckerboardScene *>(this->scene())->viewer;
}

void ClippedRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->save();
    if (parentItem()) { // 裁切
        QRectF parentRect = parentItem()->boundingRect();
        QPolygonF clipPolygon = mapFromItem(parentItem(), parentRect);
        QPainterPath clipPath;
        clipPath.addPolygon(clipPolygon);
        painter->setClipPath(clipPath);
    }

    painter->save();
    QPen pen = this->pen();
    QBrush brush = this->brush();
    painter->setPen(pen);
    painter->setBrush(brush);
    qreal width = pen.widthF();
    QRectF r = option->rect.adjusted(+width, +width, -width, -width);
    painter->drawRect(r);
    painter->restore();

    // painter->setPen(Qt::NoPen);
    // painter->setBrush(Qt::NoBrush);

    // this->setPen(Qt::NoPen);
    // this->setBrush(Qt::NoBrush);
    // QGraphicsRectItem::paint(painter, option, widget);
    // this->setPen(pen);
    // this->setBrush(brush);

    if (option->state & QStyle::State_Selected) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, false);

        QPen dashPen(Qt::SolidLine);
        // dashPen.setCosmetic(true);
        dashPen.setWidth(1);
        painter->setBrush(Qt::NoBrush);

        dashPen.setColor(Qt::white);
        painter->setPen(dashPen);
        painter->drawRect(this->rect());

        dashPen.setColor(Qt::black);
        dashPen.setStyle(Qt::DashLine);
        painter->setPen(dashPen);
        painter->drawRect(this->rect());

        painter->restore();
    }

    painter->restore();
}


QPainterPath ClippedRectItem::shape() const {
    QPainterPath path;
    QRectF r = rect();

    if (brush().color() != Qt::transparent) {
        path.addRect(rect());
        return path;
    }
    const qreal handleSize = qMax(resizeMargin, static_cast<qreal>(pen().width()));

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


ClippedRectItem::DragRegion ClippedRectItem::hitTest(const QPointF &pos) {
    if (!this->isSelected()) {
        return None;
    }

    QRectF r = rect();
    bool left = qAbs(pos.x() - r.left()) <= qMax(resizeMargin, static_cast<qreal>(pen().width()));
    bool right = qAbs(pos.x() - r.right()) <= qMax(resizeMargin, static_cast<qreal>(pen().width()));
    bool top = qAbs(pos.y() - r.top()) <= qMax(resizeMargin, static_cast<qreal>(pen().width()));
    bool bottom = qAbs(pos.y() - r.bottom()) <= qMax(resizeMargin, static_cast<qreal>(pen().width()));

    if (top && left)
        return TopLeft;
    if (top && right)
        return TopRight;
    if (bottom && left)
        return BottomLeft;
    if (bottom && right)
        return BottomRight;
    // if (top)
    //     return Top;
    // if (bottom)
    //     return Bottom;
    // if (left)
    //     return Left;
    // if (right)
    //     return Right;

    return None;
}

void ClippedRectItem::updateCursor(DragRegion region) {
    if (!this->isSelected()) {
        unsetCursor();
        return;
    }

    switch (region) {
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
        setCursor(Qt::SizeAllCursor);
        // unsetCursor();
        break;
    }
}

void ClippedRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    QPointF pos = event->pos();
    updateCursor(hitTest(pos));
    QGraphicsRectItem::hoverMoveEvent(event);
}

void ClippedRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    pressPos = event->pos();
    dragRegion = hitTest(pressPos);
    originalRect = rect();
    QGraphicsRectItem::mousePressEvent(event);
}

void ClippedRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (dragRegion == None) {
        QGraphicsRectItem::mouseMoveEvent(event);
        return;
    }

    QPointF delta = event->pos() - pressPos;
    QRectF r = originalRect;

    switch (dragRegion) {
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
    if (r.width() < minWidth)
        r.setWidth(minWidth);
    if (r.height() < minHeight)
        r.setHeight(minHeight);

    prepareGeometryChange();
    setRect(r.normalized());
}

void ClippedRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    dragRegion = None;
    QGraphicsRectItem::mouseReleaseEvent(event);
}

void ClippedRectItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    this->viewer()->selectOnlyOne(this);

    QMenu menu;
    QAction *action1 = menu.addAction("删除");

    QAction *selectedAction = menu.exec(event->screenPos());
    if (selectedAction == action1) {
        this->viewer()->scene->removeElement(this);
    }
}
