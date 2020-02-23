#include <CQNES_Sprites.h>
#include <CQNES_PPU.h>

#include <QHBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>

namespace CNES {

QPPU_Sprites::
QPPU_Sprites(QPPU *qppu) :
 qppu_(qppu)
{
  setWindowTitle("Sprites");

  setObjectName("sprites");

  //---

  auto *layout = new QHBoxLayout(this);

  auto *clayout = new QVBoxLayout;

  canvas1_ = new QPPU_SpritesCanvas(this, false);
  canvas2_ = new QPPU_SpritesCanvas(this, true );

  clayout->addWidget(canvas1_);
  clayout->addWidget(canvas2_);

  layout->addLayout(clayout);

  auto *controlFrame  = new QFrame;
  auto *controlLayout = new QGridLayout(controlFrame);

  layout->addWidget(controlFrame);

  //---

  int row = 0;

  auto addEdit = [&](const QString &name) {
    controlLayout->addWidget(new QLabel(name), row, 0);

    auto *edit = new QLineEdit;

    controlLayout->addWidget(edit, row, 1);

    ++row;

    return edit;
  };

  auto addCheck = [&](const QString &name) {
    auto *check = new QCheckBox(name);

    controlLayout->addWidget(check, row, 0, 1, 2);

    ++row;

    return check;
  };

  numEdit_    = addEdit("Num");
  xEdit_      = addEdit("X");
  yEdit_      = addEdit("Y");
  flipXCheck_ = addCheck("Flip X");
  flipYCheck_ = addCheck("Flip Y");

  controlLayout->setRowStretch(row, 1);

  //---

  connect(qppu_, SIGNAL(spritesChangedSignal()), this, SLOT(updateSlot()));
}

void
QPPU_Sprites::
updateSlot()
{
  canvas1_->update();
  canvas2_->update();

  PPU::SpriteData spriteData;

  qppu_->getSpriteData(spriteNum_, spriteData);

  numEdit_   ->setText(QString("%1").arg(spriteData.num));
  xEdit_     ->setText(QString("%1").arg(spriteData.x));
  yEdit_     ->setText(QString("%1").arg(spriteData.y));
  flipXCheck_->setChecked(spriteData.flipX);
  flipYCheck_->setChecked(spriteData.flipY);
}

//---

QPPU_SpritesCanvas::
QPPU_SpritesCanvas(QPPU_Sprites *qsprites, bool alt) :
 qsprites_(qsprites), alt_(alt)
{
}

void
QPPU_SpritesCanvas::
mousePressEvent(QMouseEvent *e)
{
  int s = qsprites_->qppu()->scale();

  int ix = std::min(std::max(e->x()/(s*8), 0), 7);
  int iy = std::min(std::max(e->y()/(s*8), 0), 7);

  int spriteNum = iy*8 + ix;

  qsprites_->setSpriteNum(spriteNum);
}

void
QPPU_SpritesCanvas::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  qsprites_->qppu()->drawSprites(&painter, alt_);
}

QSize
QPPU_SpritesCanvas::
sizeHint() const
{
  int s = qsprites_->qppu()->scale();

  return QSize(s*64, s*64);
}

}
