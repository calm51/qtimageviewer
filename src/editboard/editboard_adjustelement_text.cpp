#pragma execution_character_set("utf-8")

#include "editboard_adjustelement_text.h"
#include "ui_editboard_adjustelement_text.h"

#include <qtframeless/qtframeless.h>

#include <QColorDialog>
#include <QTextCursor>
#include <QTextDocument>

#include "../elements/clippedbluritem.h"
#include "../elements/clippedtextitem.h"
#include "../qtimageviewer.h"
#include "../scene/checkerboardscene.h"
#include "./editboard.h"

EditBoard_AdjustElement_Text::EditBoard_AdjustElement_Text(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EditBoard_AdjustElement_Text) {
    ui->setupUi(this);

    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

    ui->fontComboBox->setVisible(false);

    QFL::__global__ &fg = QFL::G();
    connect(&fg.SIGNAL, &QFL::__signal__::themeChanged, this, [this]() { this->loadthemesetting(); });
    loadthemesetting();


    /*  样式部分 =================   */

    ui->toolButton_6->setText(QChar(0xf077));
    ui->toolButton_6->setToolTip("提高堆叠顺序");
    connect(ui->toolButton_6, &QToolButton::clicked, this, [=]() {
        EditBoard *editboard = EditBoard::editboard(this);

        ClippedTextItem *item = editboard->currentItem<ClippedTextItem>();
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


    ui->toolButton->setText(QChar(0xf032));
    ui->toolButton->setToolTip("粗体");
    ui->toolButton_2->setText(QChar(0xf033));
    ui->toolButton_2->setToolTip("斜体");
    ui->toolButton_3->setText(QChar(0xf0cd));
    ui->toolButton_3->setToolTip("下划线");
    ui->toolButton_4->setText(QChar(0xf031));
    ui->toolButton_4->setToolTip("文本颜色");
    //    ui->toolButton_4->setStyleSheet("#toolButton_4{color:transparent;}");
    ui->toolButton_5->setText(QChar(0xf031));
    ui->toolButton_5->setToolTip("背景色");
    ui->toolButton_11->setText(QChar(0xf0e2));
    ui->toolButton_11->setToolTip("恢复默认");

    ui->spinBox->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    ui->spinBox->installEventFilter(this);

    connect(ui->toolButton, &QToolButton::clicked, this, [=](bool checked) { // 粗体
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedTextItem *item = editboard->currentItem<ClippedTextItem>();
        if (item) {
            QFont font = item->font();
            font.setBold(checked);
            item->setFont(font);
            this->updateToolBar();
        }
    });
    connect(ui->toolButton_2, &QToolButton::clicked, this, [=](bool checked) { // 斜体
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedTextItem *item = editboard->currentItem<ClippedTextItem>();
        if (item) {
            QFont font = item->font();
            font.setItalic(checked);
            item->setFont(font);
            this->updateToolBar();
        }
    });
    connect(ui->toolButton_3, &QToolButton::clicked, this, [=](bool checked) { // 下划线
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedTextItem *item = editboard->currentItem<ClippedTextItem>();
        if (item) {
            QFont font = item->font();
            font.setUnderline(checked);
            item->setFont(font);
            this->updateToolBar();
        }
    });
    connect(ui->toolButton_4, &QToolButton::clicked, this, [=]() { // 前进色
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedTextItem *item = editboard->currentItem<ClippedTextItem>();
        if (item) {
            QColorDialog dialog(editboard->viewer());
            dialog.setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel);
            dialog.setWindowTitle("选择颜色");
            dialog.setCurrentColor(QColor(255, 64, 0));
            if (dialog.exec() == QDialog::Accepted) {
                QColor color = dialog.selectedColor();
                item->setDefaultTextColor(color);
                this->updateToolBar();
            }
        }
    });
    connect(ui->toolButton_5, &QToolButton::clicked, this, [=]() { // 背景色
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedTextItem *item = editboard->currentItem<ClippedTextItem>();
        if (item) {
            QColorDialog dialog(editboard->viewer());
            dialog.setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel);
            dialog.setWindowTitle("选择颜色");
            dialog.setCurrentColor(QColor(252, 233, 79));
            if (dialog.exec() == QDialog::Accepted) {
                QColor color = dialog.selectedColor();

                QTextCursor cursor(item->document());
                cursor.select(QTextCursor::Document);
                QTextCharFormat fmt;
                fmt.setBackground(color);
                cursor.mergeCharFormat(fmt);

                this->updateToolBar();
            }
        }
    });


    connect(ui->toolButton_11, &QToolButton::clicked, this, [=]() { // 恢复默认
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedTextItem *item = editboard->currentItem<ClippedTextItem>();
        if (item) {
            QFont font = qApp->font();
            font.setPointSize(16);
            item->setFont(font);

            QTextCursor cursor(item->document());
            cursor.select(QTextCursor::Document);
            QTextCharFormat fmt;
            fmt.setBackground(QBrush());
            fmt.setForeground(QBrush());
            cursor.mergeCharFormat(fmt);

            item->setDefaultTextColor(Qt::red);

            this->updateToolBar();
        }
    });

    connect(ui->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int i) { // 字体大小
        EditBoard *editboard = EditBoard::editboard(this);
        ClippedTextItem *item = editboard->currentItem<ClippedTextItem>();
        if (item) {
            QFont font = item->font();
            font.setPointSize(i);
            item->setFont(font);
            this->updateToolBar();
        }
    });
}

EditBoard_AdjustElement_Text::~EditBoard_AdjustElement_Text() {
    delete ui;
}

void EditBoard_AdjustElement_Text::showEvent(QShowEvent *event) {
    ui->scrollArea->setFixedHeight(ui->widget_p2->height() + 4);
    QWidget::showEvent(event);
}


void EditBoard_AdjustElement_Text::loadthemesetting() {
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

void EditBoard_AdjustElement_Text::updateToolBar(bool setup) {
    EditBoard *editboard = EditBoard::editboard(this);
    ClippedTextItem *item = editboard->currentItem<ClippedTextItem>();
    if (!item)
        return;

    QFL::__global__ &fg = QFL::G();

    ui->toolButton_4->setStyleSheet(QString(R"(.QToolButton{color:%1;})").arg(fg.qcolor2qss(item->defaultTextColor())));

    QTextCursor cursor(item->document());
    cursor.select(QTextCursor::Document);
    ui->toolButton_5->setStyleSheet(QString(R"(.QToolButton{background:%1;}.QToolButton:disabled{background:rgba(128,128,128,32);})").arg(fg.qcolor2qss(cursor.charFormat().background().color())));


    ui->toolButton->setChecked(item->font().bold());
    ui->toolButton_2->setChecked(item->font().italic());
    ui->toolButton_3->setChecked(item->font().underline());

    ui->spinBox->blockSignals(true);
    ui->spinBox->setValue(item->font().pointSize());
    ui->spinBox->blockSignals(false);

    // if (setup) {
    //     auto cursor = mvcrate.w_messageview_inputbar->ui->textEdit->textCursor();
    //     // cursor.setCharFormat(tcfmt);
    //     cursor.mergeCharFormat(tcfmt);
    // }
}
