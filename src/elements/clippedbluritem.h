#pragma execution_character_set("utf-8")

#ifndef CLIPPEDBLURITEM_H
#define CLIPPEDBLURITEM_H

#include <QGraphicsRectItem>
#include <QTimer>

class Qtimageviewer;

class ClippedBlurItem : public QGraphicsRectItem {
  public:
    ClippedBlurItem(const QRectF& rect, QGraphicsItem* parent = nullptr);

    Qtimageviewer* viewer();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    enum DragRegion { None, TopLeft, Top, TopRight, Right, BottomRight, Bottom, BottomLeft, Left };

    void grab();
    QTimer delaying_grab;

    int radius = 15;

    static void updateBlurs(Qtimageviewer* viewer, QGraphicsItem* element); // 调整堆叠后,重新捕获

  protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;


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
    QPixmap blurred;
};


#endif // CLIPPEDBLURITEM_H
