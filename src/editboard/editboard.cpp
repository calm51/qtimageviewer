#pragma execution_character_set("utf-8")

#include "editboard.h"
#include "ui_editboard.h"

#include <qtframeless/qtframeless.h>

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>

#include "../elements/clippedrectitem.h"
#include "../elements/clippedtextitem.h"
#include "../qtimageviewer.h"
#include "../scene/checkerboardscene.h"
#include "../utils.h"
#include "./editboard_adjustelement_text.h"
#include "../elements/clippedbluritem.h"

EditBoard::EditBoard(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EditBoard) {
    ui->setupUi(this);
    this->setAutoFillBackground(true);
    this->setObjectName("background");

    QFL::__global__ &fg = QFL::G();
    connect(&fg.SIGNAL, &QFL::__signal__::themeChanged, this, [this]() { this->loadthemesetting(); });
    loadthemesetting();

    ui->stackedWidget->setCurrentIndex(0);

    QTimer::singleShot(0, this, [this]() {
        connect(viewer()->scene, &QGraphicsScene::selectionChanged, this, [this]() {
            QList<QGraphicsItem *> selected = viewer()->scene->selectedItems();
            if (selected.size() > 1) {
                for (int i = 0; i < selected.size() - 1; ++i) {
                    selected[i]->setSelected(false);
                }
            }
            switchBoard();
        });
    });
}

EditBoard::~EditBoard() {
    delete ui;
}
void EditBoard::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

EditBoard *EditBoard::editboard(QWidget *child) {
    return findParentOfType<EditBoard>(child);
}

Qtimageviewer *EditBoard::viewer() {
    return findParentOfType<Qtimageviewer>(this);
}

void EditBoard::switchBoard() {
    const auto &li = viewer()->scene->selectedItems();
    if (li.isEmpty()) {
        ui->stackedWidget->setCurrentWidget(ui->page);
        return;
    }

    {
        ClippedTextItem *item = dynamic_cast<ClippedTextItem *>(li.first());
        if (item) {
            ui->stackedWidget->setCurrentWidget(ui->page_2);
            ui->widget_2->updateToolBar(false);
            return;
        }
    }

    {
        ClippedRectItem *item = dynamic_cast<ClippedRectItem *>(li.first());
        if (item) {
            ui->stackedWidget->setCurrentWidget(ui->page_3);
            ui->widget_3->updateToolBar(false);
            return;
        }
    }

    {
        ClippedBlurItem *item = dynamic_cast<ClippedBlurItem *>(li.first());
        if (item) {
            ui->stackedWidget->setCurrentWidget(ui->page_4);
            ui->widget_4->updateToolBar(false);
            return;
        }
    }

    ui->stackedWidget->setCurrentWidget(ui->page);
}


void EditBoard::loadthemesetting() {
    QFL::__global__ &fg = QFL::G();

    QColor border_color = qApp->palette().shadow().color();

    if (fg.is_light()) {
        border_color.setAlpha(38);
    } else {
        border_color.setAlpha(120);
    }

    if (fg.is_light()) {
        QColor color1 = qApp->palette().window().color();
        color1.setAlpha(160);
        setStyleSheet(QString(R"(
.QWidget,.QScrollArea{background:transparent;}
#background{
background:%1;
border:0px solid %2;
border-top-width:1px;
}
)")
                          .arg(fg.qcolor2qss(color1), fg.qcolor2qss(border_color)));
    } else {
        QColor color1 = qApp->palette().window().color();
        color1 = color1.darker(110);
        color1.setAlpha(160);
        setStyleSheet(QString(R"(
.QWidget,.QScrollArea{background:transparent;}
#background{
background:%1;
border:0px solid %2;
border-top-width:1px;
}
)")
                          .arg(fg.qcolor2qss(color1), fg.qcolor2qss(border_color)));
    }

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

    //     ui->page_2->setStyleSheet(R"(
    //     QToolButton{
    //         font-family:FontAwesome;
    //     }
    // )");

    // if (fg.is_light()) {
    //     ui->toolButton_6->setIcon(QIcon(QPixmap(":/resource/image/addlink.png")));
    // } else {
    //     ui->toolButton_6->setIcon(QIcon(QPixmap::fromImage(CO::invertedImage(":/resource/image/addlink.png"))));
    // }

    // QPalette palette = qApp->palette();
    // QColor color = palette.color(QPalette::Normal, QPalette::Base);
    // if (fg.is_light()) {
    //     color = color.darker(105);
    // } else {
    //     color = color.darker(130);
    // }
    // ui->textEdit->setStyleSheet(QString("QTextEdit:enabled{background-color: %1;}").arg(fg.qcolor2qss(color)));
}
