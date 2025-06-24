#pragma execution_character_set("utf-8")

#ifndef CANVASRECTITEM_H
#define CANVASRECTITEM_H

#include <QGraphicsRectItem>

class Qtimageviewer;

class CanvasRectItem : public QGraphicsRectItem {
  public:
    CanvasRectItem(const QRectF& rect, Qtimageviewer* viewer);

  protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  private:
    Qtimageviewer* viewer = nullptr;
};

#endif // CANVASRECTITEM_H
