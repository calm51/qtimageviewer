#pragma execution_character_set("utf-8")

#ifndef CLIPPEDPIXMAPITEM_H
#define CLIPPEDPIXMAPITEM_H

#include <QGraphicsPixmapItem>

class ClippedPixmapItem : public QGraphicsPixmapItem {
  public:
    ClippedPixmapItem(const QPixmap& pixmap, QGraphicsItem* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  private:
};

#endif // CLIPPEDPIXMAPITEM_H
