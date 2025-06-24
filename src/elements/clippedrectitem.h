#pragma execution_character_set("utf-8")

#ifndef CLIPPEDRECTITEM_H
#define CLIPPEDRECTITEM_H

#include <QGraphicsRectItem>

class Qtimageviewer;

class ClippedRectItem : public QGraphicsRectItem {
  public:
    ClippedRectItem(const QRectF& rect, QGraphicsItem* parent = nullptr);

    Qtimageviewer* viewer();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    enum DragRegion { None, TopLeft, Top, TopRight, Right, BottomRight, Bottom, BottomLeft, Left };

    QPainterPath shape() const override;


  protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;


  private:
    DragRegion hitTest(const QPointF& pos);
    void updateCursor(DragRegion region);
    QRectF originalRect;
    QPointF pressPos;
    DragRegion dragRegion = None;

    const qreal resizeMargin = 8.0;
    const qreal minWidth = 20;
    const qreal minHeight = 20;

  private:
};

#endif // CLIPPEDRECTITEM_H
