#ifndef CQNES_PPU_H
#define CQNES_PPU_H

#include <CNES_PPU.h>
#include <QWidget>

class QPainter;

namespace CNES {

class QMachine;

class QPPU : public QWidget, public PPU {
  Q_OBJECT

  Q_PROPERTY(int  scale        READ scale          WRITE setScale       )
  Q_PROPERTY(int  margin       READ margin         WRITE setMargin      )
  Q_PROPERTY(bool showScanLine READ isShowScanLine WRITE setShowScanLine)

 public:
  QPPU(QMachine *qmachine);

  //---

  int scale() const { return scale_; }
  void setScale(int scale);

  int margin() const { return margin_; }
  void setMargin(int margin);

  bool isShowScanLine() const { return showScanLine_; }
  void setShowScanLine(bool b) { showScanLine_ = b; }

  //---

  void memChanged(ushort addr, ushort len) override { emit memChangedSignal(addr, len); }

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

 signals:
  void memChangedSignal(ushort addr, ushort len);

 private slots:
  void drawLineSlot();

 private:
  void updateImage();

 private:
  using KeyPressed = std::map<int,bool>;

  // 60Hz
  static const int s_cycleTime = 1000/s_displaySpeed;

  QMachine*  qmachine_     { nullptr };
  QTimer*    timer_        { nullptr };
  QImage*    image_        { nullptr };
  QPainter*  ipainter_     { nullptr };
  int        iw_           { 0 };
  int        ih_           { 0 };
  int        scale_        { 1 };
  int        margin_       { 0 };
  bool       showScanLine_ { false };
  bool       updateImage_  { true };
  int        lineNum_      { 0 };
  KeyPressed keyPressed_;
};

}

#endif
