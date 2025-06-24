#pragma execution_character_set("utf-8")

#ifndef CLIPPEDTEXTITEM_H
#define CLIPPEDTEXTITEM_H

#include <QGraphicsTextItem>

class Qtimageviewer;

class ClippedTextItem : public QGraphicsTextItem {
  public:
    ClippedTextItem(const QString& str, QGraphicsItem* parent = nullptr);

    Qtimageviewer* viewer();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    void focusOutEvent(QFocusEvent* event) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;


  private:
};

#endif // CLIPPEDTEXTITEM_H
