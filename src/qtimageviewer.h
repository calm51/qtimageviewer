#pragma execution_character_set("utf-8")

#ifndef QTIMAGEVIEWER_H
#define QTIMAGEVIEWER_H

#include <QAbstractGraphicsShapeItem>
#include <QTimer>
#include <QWidget>

#include <qtgllogger/qtgllogger.h>

#include "./qtimageviewer_global.h"

namespace Ui {
class Qtimageviewer;
}

class GraphicsView;
class CheckerboardScene;
class CanvasRectItem;
class HandleRectItem;
class QGraphicsView;
class ClippedPixmapItem;


class EditBoard;

class QTIMAGEVIEWER_EXPORT Qtimageviewer : public QWidget {
    Q_OBJECT

  public:
    explicit Qtimageviewer(QWidget *parent = nullptr);
    ~Qtimageviewer();

    QGL_LOGGER_OBJECT_STATIC;

    bool eventFilter(QObject *watched, QEvent *event) override;

    bool open(const QPixmap &p);
    bool open(const QByteArray &bytes);


    void setCheckerboardColor(const QColor &color1, const QColor &color2);

    bool fitInAfterResize = false; // 是否在窗口缩放后自适应图片缩放
    void fitInView();

    enum Mode { Unknown, View, Edit };
    void setMode(Mode mode);
    Mode getMode();

    QImage save();

    friend class GraphicsView;
    friend class CheckerboardScene;

    friend class CanvasRectItem;
    friend class HandleRectItem;

    friend class ClippedTextItem;
    friend class ClippedRectItem;
    friend class ClippedBlurItem;

    friend class EditBoard;
    friend class EditBoard_AddElement;
    friend class EditBoard_AdjustElement_Text;
    friend class EditBoard_AdjustElement_Rect;
    friend class EditBoard_AdjustElement_Blur;

  private:
    Ui::Qtimageviewer *ui;

    EditBoard *editboard = nullptr;
    QList<QGraphicsItem *> elements;
    void unselectAll();
    void selectOnlyOne(QGraphicsItem *item);

    Mode mode = Mode::Unknown;
    void _switch_mode();

    QPixmap pixmap; // 原图
    ClippedPixmapItem *origin = nullptr;
    void _new_origin();

    GraphicsView *view = nullptr;
    CheckerboardScene *scene = nullptr;
    CanvasRectItem *canvas = nullptr; // 画布
    HandleRectItem *handle = nullptr;
    void _new_canvas(const QSize &size);

    void showMenu(const QPoint &globalPos); // 空白处的右键

    bool panning = false; // 按住空白区域移动画布
    QPoint lastMousePos;

    QTimer _timer_fitInAfterResize;
};

#endif // QTIMAGEVIEWER_H
