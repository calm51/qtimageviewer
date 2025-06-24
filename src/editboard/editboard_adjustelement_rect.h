#pragma execution_character_set("utf-8")

#ifndef EDITBOARD_ADJUSTELEMENT_RECT_H
#define EDITBOARD_ADJUSTELEMENT_RECT_H

#include <QWidget>

namespace Ui {
class EditBoard_AdjustElement_Rect;
}

class EditBoard_AdjustElement_Rect : public QWidget {
    Q_OBJECT

  public:
    explicit EditBoard_AdjustElement_Rect(QWidget* parent = nullptr);
    ~EditBoard_AdjustElement_Rect();

    void showEvent(QShowEvent* event) override;

    void loadthemesetting();

    void updateToolBar(bool setup = true);


  private:
    Ui::EditBoard_AdjustElement_Rect* ui;
};

#endif // EDITBOARD_ADJUSTELEMENT_RECT_H
