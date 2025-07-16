#pragma execution_character_set("utf-8")

#include "editboard_addelement.h"
#include "ui_editboard_addelement.h"

#include "../elements/clippedbluritem.h"
#include "../elements/clippedrectitem.h"
#include "../elements/clippedtextitem.h"
#include "../qtimageviewer.h"
#include "../scene/canvasrectitem.h"
#include "../scene/checkerboardscene.h"
#include "../scene/graphicsview.h"
#include "../utils.h"
#include "./editboard.h"
#include <QGraphicsView>


EditBoard_AddElement::EditBoard_AddElement(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EditBoard_AddElement) {
    ui->setupUi(this);

    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

    connect(ui->toolButton, &QToolButton::clicked, this, [this]() {
        EditBoard *editboard = EditBoard::editboard(this);
        editboard->viewer()->view->endDraw();
        addRect();
    });
    connect(ui->toolButton_2, &QToolButton::clicked, this, [this]() {
        EditBoard *editboard = EditBoard::editboard(this);
        editboard->viewer()->view->endDraw();
        addText();
    });
    connect(ui->toolButton_3, &QToolButton::clicked, this, [this]() {
        EditBoard *editboard = EditBoard::editboard(this);
        editboard->viewer()->view->endDraw();
        addBlur();
    });
}

EditBoard_AddElement::~EditBoard_AddElement() {
    delete ui;
}

void EditBoard_AddElement::showEvent(QShowEvent *event) {
    ui->scrollArea->setFixedHeight(ui->toolButton->height() + 4);
    QWidget::showEvent(event);
}

void EditBoard_AddElement::addText() {
    EditBoard *editboard = EditBoard::editboard(this);

    ClippedTextItem *item = new ClippedTextItem("双击修改文本");
    item->setZValue(2);
    item->setParentItem(editboard->viewer()->canvas);

    QFont font = qApp->font();
    font.setPointSize(16);
    item->setFont(font);
    item->setDefaultTextColor(Qt::red);

    editboard->viewer()->elements.append(item);
    editboard->viewer()->unselectAll();
    item->setSelected(true);

    // 移动到视口中心
    QPoint viewCenter = editboard->viewer()->view->viewport()->rect().center();
    QPointF sceneCenter = editboard->viewer()->view->mapToScene(viewCenter);
    QPointF localCenter = editboard->viewer()->canvas->mapFromScene(sceneCenter);
    // item->setPos(localCenter);
    QRectF parentRect = editboard->viewer()->canvas->boundingRect();
    QSizeF childSize = item->boundingRect().size();
    // 居中
    QPointF topLeft = localCenter - QPointF(childSize.width() / 2, childSize.height() / 2);
    // 限制子项位置使其不会超出父项区域
    qreal minX = parentRect.left();
    qreal maxX = parentRect.right() - childSize.width();
    qreal minY = parentRect.top();
    qreal maxY = parentRect.bottom() - childSize.height();
    qreal finalX = qBound(minX, topLeft.x(), maxX);
    qreal finalY = qBound(minY, topLeft.y(), maxY);
    //
    item->setPos(finalX, finalY);

    return;
}

void EditBoard_AddElement::addRect() {
    EditBoard *editboard = EditBoard::editboard(this);

    ClippedRectItem *item = new ClippedRectItem(QRect(0, 0, 20, 20));
    item->setZValue(2);
    item->setParentItem(editboard->viewer()->canvas);

    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(3);
    item->setPen(pen);
    QBrush brush;
    brush.setColor(Qt::transparent);
    item->setBrush(brush);

    editboard->viewer()->elements.append(item);
    editboard->viewer()->unselectAll();
    item->setSelected(true);

    item->setVisible(false); // 先不可见
    editboard->viewer()->view->startDraw(item);


}


void EditBoard_AddElement::addBlur() {
    EditBoard *editboard = EditBoard::editboard(this);

    ClippedBlurItem *item = new ClippedBlurItem(QRect(0, 0, 20, 20));
    item->setZValue(2);
    item->setParentItem(editboard->viewer()->canvas);

    item->viewer()->connect(&item->delaying_grab, &QTimer::timeout, &item->delaying_grab, [=]() { item->grab(); });

    editboard->viewer()->elements.append(item);
    editboard->viewer()->unselectAll();
    item->setSelected(true);

    item->setVisible(false); // 先不可见
    editboard->viewer()->view->startDraw(item);
}
