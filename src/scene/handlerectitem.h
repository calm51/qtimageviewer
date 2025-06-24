#pragma execution_character_set("utf-8")

#ifndef HANDLERECTITEM_H
#define HANDLERECTITEM_H

#include <QGraphicsRectItem>

class Qtimageviewer;

class HandleRectItem : public QGraphicsRectItem {
  public:
    enum Handle { None, TopLeft, Top, TopRight, Right, BottomRight, Bottom, BottomLeft, Left };

    HandleRectItem(const QRectF& rect, Qtimageviewer* viewer);

  protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    QPainterPath shape() const override;

  private:
    Handle handle = None;
    QRectF origRect;
    QPointF origMousePos;

    void updateCursor(const QPointF& pos);

    Handle detectHandle(const QPointF& pos) const;

    QList<QPointF> handlePoints() const;

    Qtimageviewer* viewer = nullptr;
};

#endif // HANDLERECTITEM_H
