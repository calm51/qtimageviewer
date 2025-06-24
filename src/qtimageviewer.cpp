#pragma execution_character_set("utf-8")

#include "qtimageviewer.h"
#include "ui_qtimageviewer.h"

#include "./elements/clippedpixmapitem.h"
#include "./scene/canvasrectitem.h"
#include "./scene/checkerboardscene.h"
#include "./scene/handlerectitem.h"

#include "./editboard/editboard.h"

#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QImage>
#include <QMenu>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QPainter>
#include <QScrollBar>

Qtimageviewer::Qtimageviewer(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Qtimageviewer) {
    ui->setupUi(this);

    scene = new CheckerboardScene(); // 场景
    scene->setSceneRect(0, 0, 10, 10);
    scene->viewer = this;

    view = ui->graphicsView; // 视图
    // view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    // view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    // view->setDragMode(QGraphicsView::RubberBandDrag);
    // view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setDragMode(QGraphicsView::NoDrag);
    view->setScene(scene);
    view->installEventFilter(this);
    view->viewport()->installEventFilter(this);

    view->setRenderHint(QPainter::SmoothPixmapTransform, true); // 高质量缩放
    view->setRenderHint(QPainter::Antialiasing, true);          // 抗锯齿

    view->_new_line();

    // QOpenGLWidget* glViewport = new QOpenGLWidget();
    // view->setViewport(glViewport);
    // QGL_DEBUG(view->viewport());


    _timer_fitInAfterResize.setSingleShot(true);
    _timer_fitInAfterResize.setInterval(60);
    connect(&_timer_fitInAfterResize, &QTimer::timeout, this, [this]() { this->fitInView(); });


    QGridLayout* layout = qobject_cast<QGridLayout*>(this->layout());

    editboard = new EditBoard(this);
    layout->addWidget(this->editboard, 0, 0, 1, 1, Qt::AlignBottom);
}

Qtimageviewer::~Qtimageviewer() {
    _timer_fitInAfterResize.stop();
    if (origin) {
        delete origin;
        origin = nullptr;
    }
    if (handle) {
        delete handle;
        handle = nullptr;
    }
    if (canvas) {
        delete canvas;
        canvas = nullptr;
    }
    if (scene) {
        delete scene;
        scene = nullptr;
    }
    if (editboard) {
        delete editboard;
        editboard = nullptr;
    }
    delete ui;
    view = nullptr;
}


bool Qtimageviewer::eventFilter(QObject* watched, QEvent* event) {
    if (watched == view) {
        if (event->type() == QEvent::Wheel) {
            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            const double scaleFactor = 1.15;
            double factor = wheelEvent->angleDelta().y() > 0 ? scaleFactor : 1.0 / scaleFactor;
            double currentScale = view->transform().m11();
            double newScale = currentScale * factor;
            // QGL_DEBUG(currentScale, newScale);
            if ((newScale < 0.1 && newScale < currentScale) || (newScale > 10 && newScale > currentScale)) {
                event->accept();
                return true;
            }
            view->scale(factor, factor);
            event->accept();
            return true;
        } else if (event->type() == QEvent::Show) {
            fitInView();
        } else if (event->type() == QEvent::Resize) {
            if (fitInAfterResize) {
                _timer_fitInAfterResize.start();
            }
        }
    }
    if (watched == view->viewport()) {
        if (!canvas)
            return false;

        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* ev = static_cast<QMouseEvent*>(event);
            if (ev->button() == Qt::LeftButton) {
                QPointF scenePos = view->mapToScene(ev->pos());
                QGraphicsItem* item = view->scene()->itemAt(scenePos, view->transform());

                if (!item) { // 点击空白区域
                    unselectAll();
                    panning = true;
                    lastMousePos = ev->pos();
                    view->viewport()->setCursor(Qt::ClosedHandCursor);
                    return true;
                }
            } else {
                if (view->drawingItem) {
                } else {
                    QPointF scenePos = view->mapToScene(ev->pos());
                    QGraphicsItem* item = view->scene()->itemAt(scenePos, view->transform());

                    if (ev->button() == Qt::RightButton && elements.contains(item)) {
                    } else {
                        unselectAll();
                        panning = true;
                        lastMousePos = ev->pos();
                        view->viewport()->setCursor(Qt::ClosedHandCursor);
                        return true;
                    }
                }
            }

        } else if (event->type() == QEvent::MouseMove) {
            if (panning) {
                QMouseEvent* ev = static_cast<QMouseEvent*>(event);
                QPoint delta = ev->pos() - lastMousePos;
                lastMousePos = ev->pos();

                // 将 delta 映射为场景坐标系移动量
                QPointF deltaScene = view->mapToScene(ev->pos()) - view->mapToScene(ev->pos() - delta);

                // 移动 bgRect（你可以根据需要移动整个 group item）
                canvas->moveBy(deltaScene.x(), deltaScene.y());

                return true;
            }

        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* ev = static_cast<QMouseEvent*>(event);
            if (panning) {
                panning = false;
                view->viewport()->unsetCursor();
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

bool Qtimageviewer::open(const QPixmap& p) {
    if (p.isNull())
        return false;

    // QImage img = p.toImage().convertToFormat(QImage::Format_ARGB32);
    // pixmap = QPixmap::fromImage(img);
    pixmap = p;

    _new_canvas(pixmap.size());
    _new_origin();
    fitInView();

    _switch_mode();
    return true;
}

bool Qtimageviewer::open(const QByteArray& bytes) {
    QPixmap p;
    p.loadFromData(bytes);
    return open(p);
}

void Qtimageviewer::setCheckerboardColor(const QColor& color1, const QColor& color2) {
    scene->color1 = color1;
    scene->color2 = color2;
}

void Qtimageviewer::fitInView() {
    // 缩放到合适比例并居中
    QRectF itemsRect = view->scene()->itemsBoundingRect();
    view->fitInView(itemsRect, Qt::KeepAspectRatio);
    // view->centerOn(bgRect);

    // QRectF rect = bgRect->sceneBoundingRect();
    // qreal centerX = rect.left() + rect.width() / 2;
    // qreal centerY = rect.top() + rect.height() / 2;
    // view->centerOn(centerX, centerY);

    if (!canvas) {
        return;
    }

    QRectF viewRect = view->mapToScene(view->viewport()->rect()).boundingRect();
    QPointF viewCenter = viewRect.center();
    QRectF itemRect = canvas->sceneBoundingRect();
    QPointF itemCenter = itemRect.center();
    QPointF offset = viewCenter - itemCenter;
    canvas->moveBy(offset.x(), offset.y());
}

void Qtimageviewer::setMode(Mode mode) {
    this->mode = mode;
    _switch_mode();
}

Qtimageviewer::Mode Qtimageviewer::getMode() {
    return mode;
}

QImage Qtimageviewer::save() {
    bool _is_edit = false;
    if (mode == Edit) {
        _is_edit = true;
        setMode(Mode::View);
    }
    this->unselectAll();
    QRectF cropRect = canvas->sceneBoundingRect();
    QImage image(cropRect.size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    scene->render(&painter, QRectF(image.rect()), cropRect);
    painter.end();

    if (_is_edit) {
        setMode(Mode::Edit);
    }

    return image;
}

void Qtimageviewer::_new_origin() {
    origin = new ClippedPixmapItem(pixmap);
    origin->setFlags(QGraphicsItem::ItemIsMovable);
    origin->setPos(canvas->rect().center() - QPointF(pixmap.width() / 2, pixmap.height() / 2));
    origin->setZValue(1);
    // origin->setFlag(QGraphicsItem::ItemIsMovable, false);
    // origin->setAcceptedMouseButtons(Qt::NoButton);
    origin->setParentItem(canvas);
}

void Qtimageviewer::_new_canvas(const QSize& size) {
    canvas = new CanvasRectItem(QRectF(0, 0, size.width(), size.height()), this); // 背景
    canvas->setBrush(Qt::white);
    canvas->setZValue(0);
    canvas->setFlag(QGraphicsItem::ItemIsSelectable, false);

    handle = new HandleRectItem(canvas->rect().adjusted(-8, -8, 8, 8), this);
    handle->setParentItem(canvas);
    handle->setZValue(99);

    scene->addItem(canvas);
}

void Qtimageviewer::unselectAll() {
    for (QGraphicsItem* item : scene->selectedItems()) {
        item->setSelected(false);
    }
    scene->setFocusItem(nullptr);
}

void Qtimageviewer::selectOnlyOne(QGraphicsItem* item) {
    unselectAll();
    item->setSelected(true);
}

void Qtimageviewer::showMenu(const QPoint& globalPos) {
    QMenu menu;

    if (menu.actions().isEmpty()) {
        return;
    }
    menu.exec(globalPos);
}

void Qtimageviewer::_switch_mode() {
    if (origin) {
        if (mode == Edit) {
            // origin->setAcceptedMouseButtons(Qt::LeftButton);
            // origin->setFlag(QGraphicsItem::ItemIsSelectable, true);
            origin->setAcceptedMouseButtons(Qt::NoButton);
            origin->setFlag(QGraphicsItem::ItemIsSelectable, false);
            origin->unsetCursor();
        } else {
            origin->setAcceptedMouseButtons(Qt::NoButton);
            origin->setFlag(QGraphicsItem::ItemIsSelectable, false);
            origin->setCursor(Qt::OpenHandCursor);
        }
    }
    if (canvas) {
        if (mode == Edit) {
            canvas->setFlag(QGraphicsItem::ItemIsMovable, false);
            canvas->unsetCursor();
        } else {
            canvas->setFlag(QGraphicsItem::ItemIsMovable, true);
            canvas->setCursor(Qt::OpenHandCursor);
        }
    }
    if (handle) {
        if (mode == Edit) {
            handle->setFlag(QGraphicsItem::ItemIsSelectable, true);
            handle->setFlag(QGraphicsItem::ItemIsMovable, false);
            handle->setVisible(true);
        } else {
            handle->setVisible(false);
        }
    }
    if (editboard) {
        if (mode == Edit) {
            editboard->setVisible(true);
        } else {
            editboard->setVisible(false);
        }
    }
}
