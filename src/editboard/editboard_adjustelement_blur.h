#pragma execution_character_set("utf-8")

#ifndef EDITBOARD_ADJUSTELEMENT_BLUR_H
#define EDITBOARD_ADJUSTELEMENT_BLUR_H

#include <QWidget>

namespace Ui {
class EditBoard_AdjustElement_Blur;
}

class EditBoard_AdjustElement_Blur : public QWidget {
    Q_OBJECT

  public:
    explicit EditBoard_AdjustElement_Blur(QWidget* parent = nullptr);
    ~EditBoard_AdjustElement_Blur();

    void showEvent(QShowEvent* event) override;

    void loadthemesetting();

    void updateToolBar(bool setup = true);


  private:
    Ui::EditBoard_AdjustElement_Blur* ui;
};

#endif // EDITBOARD_ADJUSTELEMENT_Blur_H
