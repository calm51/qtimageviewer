#pragma execution_character_set("utf-8")

#ifndef CHECKERBOARDSCENE_H
#define CHECKERBOARDSCENE_H

#include <QGraphicsScene>
#include <QObject>

class GraphicsView;
class Qtimageviewer;

class CheckerboardScene : public QGraphicsScene {
    Q_OBJECT
  public:
    explicit CheckerboardScene(QObject* parent = nullptr);

    QColor color1 = QColor(220, 220, 220);
    QColor color2 = QColor(180, 180, 180);

    Qtimageviewer* viewer = nullptr;

    void removeElement(QGraphicsItem* element);

  protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
};

#endif // CHECKERBOARDSCENE_H
