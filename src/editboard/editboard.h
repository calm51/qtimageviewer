#pragma execution_character_set("utf-8")

#ifndef EDITBOARD_H
#define EDITBOARD_H

#include <QGraphicsItem>
#include <QWidget>

#include "../qtimageviewer.h"
#include "../scene/checkerboardscene.h"

namespace Ui {
class EditBoard;
}

// class Qtimageviewer;

class EditBoard : public QWidget {
    Q_OBJECT

  public:
    explicit EditBoard(QWidget *parent = nullptr);
    ~EditBoard();

    void loadthemesetting();

    void paintEvent(QPaintEvent *) override;

    static EditBoard *editboard(QWidget *child);
    Qtimageviewer *viewer();

    void switchBoard();

    template<typename T>
    T *currentItem() {
        const auto &li = viewer()->scene->selectedItems();
        if (li.isEmpty()) {
            return nullptr;
        }
        return dynamic_cast<T *>(li.first());
    }

  private:
    Ui::EditBoard *ui;
};

#endif // EDITBOARD_H
