#pragma execution_character_set("utf-8")

#ifndef EDITBOARD_ADDELEMENT_H
#define EDITBOARD_ADDELEMENT_H

#include <QWidget>

namespace Ui {
class EditBoard_AddElement;
}

class EditBoard_AddElement : public QWidget {
    Q_OBJECT

  public:
    explicit EditBoard_AddElement(QWidget *parent = nullptr);
    ~EditBoard_AddElement();

    void showEvent(QShowEvent *event) override;

    void addText();

    void addRect();

  private:
    Ui::EditBoard_AddElement *ui;
};

#endif // EDITBOARD_ADDELEMENT_H
