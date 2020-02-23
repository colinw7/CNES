#ifndef CQNES_Sprites_H
#define CQNES_Sprites_H

#include <CNES_PPU.h>
#include <QWidget>

class QLineEdit;
class QCheckBox;

namespace CNES {

class QPPU;
class QPPU_SpritesCanvas;

class QPPU_Sprites : public QWidget {
  Q_OBJECT

  Q_PROPERTY(int spriteNum READ spriteNum WRITE setSpriteNum)

 public:
  QPPU_Sprites(QPPU *ppu);

  QPPU *qppu() const { return qppu_; }

  int spriteNum() const { return spriteNum_; }
  void setSpriteNum(int i) { spriteNum_ = i; updateSlot(); }

 private slots:
  void updateSlot();

 private:
  QPPU*               qppu_       { nullptr };
  QPPU_SpritesCanvas* canvas1_    { nullptr };
  QPPU_SpritesCanvas* canvas2_    { nullptr };
  QLineEdit*          numEdit_    { nullptr };
  QLineEdit*          xEdit_      { nullptr };
  QLineEdit*          yEdit_      { nullptr };
  QCheckBox*          flipXCheck_ { nullptr };
  QCheckBox*          flipYCheck_ { nullptr };
  int                 spriteNum_  { 0 };
};

//---

class QPPU_SpritesCanvas : public QWidget {
  Q_OBJECT

 public:
  QPPU_SpritesCanvas(QPPU_Sprites *sprites, bool alt);

  void mousePressEvent(QMouseEvent *e) override;

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  QPPU_Sprites* qsprites_ { nullptr };
  bool          alt_      { false };
};

}

#endif
