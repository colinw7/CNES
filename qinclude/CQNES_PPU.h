#ifndef CQNES_PPU_H
#define CQNES_PPU_H

#include <CNES_PPU.h>
#include <QWidget>

class QPainter;

namespace CNES {

class QMachine;

class QPPU : public QWidget, public PPU {
  Q_OBJECT

 public:
  QPPU(QMachine *qmachine);

  int scale() const { return scale_; }
  void setScale(int scale);

  int margin() const { return margin_; }
  void setMargin(int margin);

  bool isKey1(int n) override;
  bool isKey2(int n) override;

  //---

  void resizeEvent(QResizeEvent *) override;
  void paintEvent(QPaintEvent *) override;

  void keyPressEvent  (QKeyEvent *e) override;
  void keyReleaseEvent(QKeyEvent *e) override;

  //---

  void setColor(uchar c) override;
  void drawPixel(int x, int y) override;

  //---

  QSize sizeHint() const override;

 private slots:
  void drawLineSlot();

 private:
  void updateImage();

 private:
  using KeyPressed = std::map<int,bool>;

  QMachine*  qmachine_    { nullptr };
  QTimer*    timer_       { nullptr };
  QImage*    image_       { nullptr };
  QPainter*  ipainter_    { nullptr };
  int        margin_      { 4 };
  int        scale_       { 1 };
  bool       updateImage_ { true };
  int        lineNum_     { 0 };
  KeyPressed keyPressed_;
};

}

#endif
