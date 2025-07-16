#pragma execution_character_set("utf-8")

#include "clippedbluritem.h"

#include <qaction.h>
#include "../qtimageviewer.h"
#include "../scene/canvasrectitem.h"
#include "../scene/checkerboardscene.h"
#include "../scene/graphicsview.h"
#include <QBuffer>
#include <QCursor>
#include <QGraphicsBlurEffect>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTextCursor>
#include <QTextDocument>


ClippedBlurItem::ClippedBlurItem(const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent) {
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    setCursor(Qt::SizeAllCursor);
    setAcceptHoverEvents(true);

    this->setPen(Qt::NoPen);
    this->setBrush(Qt::black);

    delaying_grab.setInterval(60);
    delaying_grab.setSingleShot(true);
}

Qtimageviewer *ClippedBlurItem::viewer() {
    return qobject_cast<CheckerboardScene *>(this->scene())->viewer;
}

void ClippedBlurItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
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
    if (blurred.isNull()) {
        painter->drawRect(r);
    } else {
        painter->drawPixmap(r, blurred, blurred.rect());
    }
    painter->restore();

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


ClippedBlurItem::DragRegion ClippedBlurItem::hitTest(const QPointF &pos) {
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
    if (top)
        return Top;
    if (bottom)
        return Bottom;
    if (left)
        return Left;
    if (right)
        return Right;

    return None;
}

void ClippedBlurItem::updateCursor(DragRegion region) {
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

void ClippedBlurItem::grab() {
    bool is_selected = this->isSelected();

    // 隐藏高层元素
    QList<QGraphicsItem *> hidden;
    QList<QGraphicsItem *> li = this->viewer()->canvas->childItems();
    for (int i = li.length() - 1; i > -1; --i) {
        if (li[i] == this) {
            li[i]->setVisible(false);
            break;
        }
        if (li[i]->isVisible()) {
            li[i]->setVisible(false);
            hidden.append(li[i]);
        }
    }

    // 截图
    int blurRadius = radius;
    int margin = blurRadius * 2;
    QRectF sceneRect = this->mapToScene(this->rect()).boundingRect();
    QRectF paddedRect = sceneRect.adjusted(-margin, -margin, margin, margin);
    QImage image(paddedRect.size().toSize(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    this->scene()->render(&painter, QRectF(image.rect()), paddedRect);
    painter.end();
    // {
    //     QByteArray byteArray;
    //     QBuffer buffer(&byteArray);
    //     buffer.open(QIODevice::WriteOnly);
    //     image.save(&buffer, "JPEG", 80);
    //     QImage lowQualityImage;
    //     lowQualityImage.loadFromData(byteArray, "JPEG");
    //     image = lowQualityImage;
    // }
    static QLabel blur_label;
    static QGraphicsBlurEffect blur;
    blur.setBlurHints(QGraphicsBlurEffect::PerformanceHint);
    blur.setBlurRadius(blurRadius);
    blur_label.setGraphicsEffect(&blur);
    blur_label.resize(image.width(), image.height());
    blur_label.setPixmap(QPixmap::fromImage(image));
    QPixmap final = blur_label.grab().copy(margin, margin, sceneRect.width(), sceneRect.height());
    // final.save("./blured.png");
    {
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        final.save(&buffer, "JPEG", 50);
        QPixmap lowQualityImage;
        lowQualityImage.loadFromData(byteArray, "JPEG");
        final = lowQualityImage;
    }

    blurred = final;

    // 恢复显示
    for (int i = hidden.length() - 1; i > -1; --i) {
        hidden[i]->setVisible(true);
    }
    this->setVisible(true);
    if (is_selected) {
        this->viewer()->selectOnlyOne(this);
    }
}

void ClippedBlurItem::updateBlurs(Qtimageviewer *viewer, QGraphicsItem *element) {
    QList<QGraphicsItem *> li = viewer->canvas->childItems();
    for (int i = 0; i < li.length(); ++i) {
        ClippedBlurItem *item = dynamic_cast<ClippedBlurItem *>(li[i]);
        if (item) {
            item->grab();
        }
    }
}

void ClippedBlurItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    QPointF pos = event->pos();
    updateCursor(hitTest(pos));
    QGraphicsRectItem::hoverMoveEvent(event);
}

void ClippedBlurItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    pressPos = event->pos();
    dragRegion = hitTest(pressPos);
    originalRect = rect();
    QGraphicsRectItem::mousePressEvent(event);
}

void ClippedBlurItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
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

void ClippedBlurItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    dragRegion = None;
    QGraphicsRectItem::mouseReleaseEvent(event);
    grab();
}

void ClippedBlurItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    this->viewer()->selectOnlyOne(this);

    QMenu menu;
    QAction *action1 = menu.addAction("删除");

    QAction *selectedAction = menu.exec(event->screenPos());
    if (selectedAction == action1) {
        this->viewer()->scene->removeElement(this);
    }
}

QVariant ClippedBlurItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    // qDebug() << change;
    // switch (change) {
    // case QGraphicsItem::ItemPositionChange:
    //     if (!delaying_grab.isActive()) {
    //         delaying_grab.start();
    //     }
    //     break;
    // default:
    //     break;
    // }
    return QGraphicsRectItem::itemChange(change, value);
}
