#pragma execution_character_set("utf-8")


#ifndef EDITBOARD_ADJUSTELEMENT_TEXT_H
#define EDITBOARD_ADJUSTELEMENT_TEXT_H

#include <QWidget>

namespace Ui {
class EditBoard_AdjustElement_Text;
}

class EditBoard_AdjustElement_Text : public QWidget {
    Q_OBJECT

  public:
    explicit EditBoard_AdjustElement_Text(QWidget *parent = nullptr);
    ~EditBoard_AdjustElement_Text();

    void showEvent(QShowEvent *event) override;

    void loadthemesetting();

    void updateToolBar(bool setup = true);


  private:
    Ui::EditBoard_AdjustElement_Text *ui;
};

#endif // EDITBOARD_ADJUSTELEMENT_TEXT_H
