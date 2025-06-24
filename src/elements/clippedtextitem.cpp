#pragma execution_character_set("utf-8")

#include "clippedtextitem.h"

#include <qaction.h>
#include "../qtimageviewer.h"
#include "../scene/checkerboardscene.h"
#include "../scene/graphicsview.h"
#include <QCursor>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QTextCursor>
#include <QTextDocument>


ClippedTextItem::ClippedTextItem(const QString &str, QGraphicsItem *parent)
    : QGraphicsTextItem(str, parent) {
    setFlags(QGraphicsItem::ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
    setTextInteractionFlags(Qt::NoTextInteraction); // 默认不可编辑
}

Qtimageviewer *ClippedTextItem::viewer() {
    return qobject_cast<CheckerboardScene *>(this->scene())->viewer;
}

void ClippedTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->save();
    if (parentItem()) { // 裁切
        QRectF parentRect = parentItem()->boundingRect();
        QPolygonF clipPolygon = mapFromItem(parentItem(), parentRect);
        QPainterPath clipPath;
        clipPath.addPolygon(clipPolygon);
        painter->setClipPath(clipPath);
    }
    QGraphicsTextItem::paint(painter, option, widget);
    painter->restore();
}

void ClippedTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setCursor(Qt::IBeamCursor);
    setFocus(Qt::MouseFocusReason);
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void ClippedTextItem::focusOutEvent(QFocusEvent *event) {
    QTextCursor cursor = this->textCursor();
    cursor.clearSelection();
    this->setTextCursor(cursor);
    setTextInteractionFlags(Qt::NoTextInteraction);
    unsetCursor();
    clearFocus();
    QGraphicsTextItem::focusOutEvent(event);
}

void ClippedTextItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    this->viewer()->selectOnlyOne(this);

    QMenu menu;
    QAction *action1 = menu.addAction("删除");

    QAction *selectedAction = menu.exec(event->screenPos());
    if (selectedAction == action1) {
        this->viewer()->scene->removeElement(this);
    }
}
