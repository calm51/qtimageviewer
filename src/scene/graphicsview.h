#pragma execution_character_set("utf-8")

#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QObject>

class Qtimageviewer;

class GraphicsView : public QGraphicsView {
    Q_OBJECT
  public:
    GraphicsView(QWidget* parent = nullptr);
    ~GraphicsView();

    void startDraw(QGraphicsRectItem* drawingItem);
    void endDraw();

    Qtimageviewer* viewer();

    void _new_line();
    friend class Qtimageviewer;

  protected:
    void mousePressEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

  private:
    QPointF startPoint;
    bool drawing;
    QGraphicsRectItem* drawingItem = nullptr;

    QGraphicsLineItem* hLine = nullptr;
    QGraphicsLineItem* vLine = nullptr;
};

#endif // GRAPHICSVIEW_H
