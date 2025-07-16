#pragma execution_character_set("utf-8")

#include "editboard_adjustelement_blur.h"
#include "ui_editboard_adjustelement_blur.h"

#include <qtframeless/qtframeless.h>

#include <QColorDialog>
#include <QTextCursor>
#include <QTextDocument>

#include "../elements/clippedbluritem.h"
#include "../qtimageviewer.h"
#include "../scene/checkerboardscene.h"
#include "./editboard.h"

#include "../qtgllogger_obj.h"

EditBoard_AdjustElement_Blur::EditBoard_AdjustElement_Blur(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EditBoard_AdjustElement_Blur) {
    ui->setupUi(this);

    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

    QFL::__global__ &fg = QFL::G();
    connect(&fg.SIGNAL, &QFL::__signal__::themeChanged, this, [this]() { this->loadthemesetting(); });
    loadthemesetting();


    /*  样式部分 =================   */

    ui->toolButton_6->setText(QChar(0xf077));
    ui->toolButton_6->setToolTip("提高堆叠顺序");
    connect(ui->toolButton_6, &QToolButton::clicked, this, [=]() {
        EditBoard *editboard = EditBoard::editboard(this);

        ClippedBlurItem *item = editboard->currentItem<ClippedBlurItem>();
        if (item) {
            for (auto *i : editboard->viewer()->elements) {
                if (i != item) {
                    i->stackBefore(item);
                }
            }
            ClippedBlurItem::updateBlurs(item->viewer(), item);
            item->viewer()->selectOnlyOne(item);
            item->scene()->update();
        }
    });

    ui->toolButton_11->setText(QChar(0xf0e2));
    ui->toolButton_11->setToolTip("恢复默认");

    ui->spinBox->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    ui->spinBox->installEventFilter(this);


    connect(ui->toolButton_11, &QToolButton::clicked, this, [=]() { // 恢复默认
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedBlurItem *item = editboard->currentItem<ClippedBlurItem>();
        if (item) {
            item->radius = 15;
            if (!item->delaying_grab.isActive()) {
                item->delaying_grab.start();
            }
            this->updateToolBar();
        }
    });

    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int i) { // 强度
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedBlurItem *item = editboard->currentItem<ClippedBlurItem>();
        if (item) {
            item->radius = i;
            if (!item->delaying_grab.isActive()) {
                item->delaying_grab.start();
            }
            this->updateToolBar();
        }
    });
}

EditBoard_AdjustElement_Blur::~EditBoard_AdjustElement_Blur() {
    delete ui;
}

void EditBoard_AdjustElement_Blur::showEvent(QShowEvent *event) {
    ui->scrollArea->setFixedHeight(ui->widget_p2->height() + 4);
    QWidget::showEvent(event);
}


void EditBoard_AdjustElement_Blur::loadthemesetting() {
    QFL::__global__ &fg = QFL::G();

    //     if (fg.is_light()) {
    //         QColor color1 = this->palette().window().color();
    //         ui->widget_p2->setStyleSheet(QString(R"(
    // .QWidget#background{
    // background:%1;
    // }
    // )")
    //                                          .arg(fg.qcolor2qss(color1)));
    //     } else {
    //         QColor color1 = this->palette().window().color();
    //         color1 = color1.darker(110);
    //         ui->widget_p2->setStyleSheet(QString(R"(
    // .QWidget#background{
    // background:%1;
    // }

    // )")
    //                                          .arg(fg.qcolor2qss(color1)));
    //     }

    setStyleSheet(R"(
        QToolButton{
            font-family: FontAwesome;
        }
    )");
}

void EditBoard_AdjustElement_Blur::updateToolBar(bool setup) {
    EditBoard *editboard = EditBoard::editboard(this);
    ClippedBlurItem *item = editboard->currentItem<ClippedBlurItem>();
    if (!item)
        return;

    QFL::__global__ &fg = QFL::G();

    ui->spinBox->blockSignals(true);
    ui->spinBox->setValue(item->radius);
    ui->spinBox->blockSignals(false);

    // if (setup) {
    //     auto cursor = mvcrate.w_messageview_inputbar->ui->textEdit->textCursor();
    //     // cursor.setCharFormat(tcfmt);
    //     cursor.mergeCharFormat(tcfmt);
    // }
}
