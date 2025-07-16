#pragma execution_character_set("utf-8")

#include "editboard_adjustelement_rect.h"
#include "ui_editboard_adjustelement_rect.h"

#include <qtframeless/qtframeless.h>

#include <QColorDialog>
#include <QTextCursor>
#include <QTextDocument>

#include "../elements/clippedbluritem.h"
#include "../elements/clippedrectitem.h"
#include "../qtimageviewer.h"
#include "../scene/checkerboardscene.h"
#include "./editboard.h"

#include "../qtgllogger_obj.h"

EditBoard_AdjustElement_Rect::EditBoard_AdjustElement_Rect(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EditBoard_AdjustElement_Rect) {
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

        ClippedRectItem *item = editboard->currentItem<ClippedRectItem>();
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

    ui->toolButton_4->setText(QChar(0xf031));
    ui->toolButton_4->setToolTip("描边颜色");
    //    ui->toolButton_4->setStyleSheet("#toolButton_4{color:transparent;}");
    ui->toolButton_5->setText(QChar(0xf031));
    ui->toolButton_5->setToolTip("填充色");
    ui->toolButton_11->setText(QChar(0xf0e2));
    ui->toolButton_11->setToolTip("恢复默认");

    ui->spinBox->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    ui->spinBox->installEventFilter(this);

    connect(ui->toolButton_4, &QToolButton::clicked, this, [=]() { // 前进色
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedRectItem *item = editboard->currentItem<ClippedRectItem>();
        if (item) {
            // qDebug()<<(editboard->viewer());
            QColorDialog dialog(editboard->viewer());
            dialog.setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel);
            dialog.setWindowTitle("选择颜色");
            dialog.setCurrentColor(QColor(255, 64, 0, 255));
            if (dialog.exec() == QDialog::Accepted) {
                QColor color = dialog.selectedColor();
                QPen pen = item->pen();
                pen.setColor(color);
                item->setPen(pen);
                this->updateToolBar();
            }
        }
    });
    connect(ui->toolButton_5, &QToolButton::clicked, this, [=]() { // 背景色
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedRectItem *item = editboard->currentItem<ClippedRectItem>();
        if (item) {
            QColorDialog dialog(editboard->viewer());
            dialog.setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel);
            dialog.setWindowTitle("选择颜色");
            dialog.setCurrentColor(QColor(0, 0, 0, 255));
            if (dialog.exec() == QDialog::Accepted) {
                QColor color = dialog.selectedColor();
                QBrush brush = item->brush();
                brush.setStyle(Qt::SolidPattern);
                brush.setColor(color);
                item->setBrush(brush);

                this->updateToolBar();
            }
        }
    });


    connect(ui->toolButton_11, &QToolButton::clicked, this, [=]() { // 恢复默认
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedRectItem *item = editboard->currentItem<ClippedRectItem>();
        if (item) {
            QPen pen;
            pen.setColor(Qt::red);
            pen.setWidth(3);
            item->setPen(pen);
            QBrush brush;
            brush.setColor(Qt::transparent);
            item->setBrush(brush);

            this->updateToolBar();
        }
    });

    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int i) { // 描边宽度
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedRectItem *item = editboard->currentItem<ClippedRectItem>();
        if (item) {
            if (i == 0) {
                item->setPen(Qt::NoPen);
            } else {
                QPen pen = item->pen();
                pen.setStyle(Qt::SolidLine);
                pen.setWidth(i);
                item->setPen(pen);
            }
            this->updateToolBar();
        }
    });
}

EditBoard_AdjustElement_Rect::~EditBoard_AdjustElement_Rect() {
    delete ui;
}

void EditBoard_AdjustElement_Rect::showEvent(QShowEvent *event) {
    ui->scrollArea->setFixedHeight(ui->widget_p2->height() + 4);
    QWidget::showEvent(event);
}


void EditBoard_AdjustElement_Rect::loadthemesetting() {
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

void EditBoard_AdjustElement_Rect::updateToolBar(bool setup) {
    EditBoard *editboard = EditBoard::editboard(this);
    ClippedRectItem *item = editboard->currentItem<ClippedRectItem>();
    if (!item)
        return;

    QFL::__global__ &fg = QFL::G();

    ui->toolButton_4->setStyleSheet(QString(R"(.QToolButton{color:%1;})").arg(fg.qcolor2qss(item->pen().color())));

    ui->toolButton_5->setStyleSheet(QString(R"(.QToolButton{background:%1;}.QToolButton:disabled{background:rgba(128,128,128,32);})").arg(fg.qcolor2qss(item->brush().color())));

    ui->spinBox->blockSignals(true);
    if (item->pen() == Qt::NoPen) {
        ui->spinBox->setValue(0);
    } else {
        ui->spinBox->setValue(item->pen().width());
    }
    ui->spinBox->blockSignals(false);

    // if (setup) {
    //     auto cursor = mvcrate.w_messageview_inputbar->ui->textEdit->textCursor();
    //     // cursor.setCharFormat(tcfmt);
    //     cursor.mergeCharFormat(tcfmt);
    // }
}
