#include <CQNES_PPU.h>
#include <CQNES_Machine.h>

#include <QTimer>
#include <QImage>
#include <QPainter>
#include <QKeyEvent>

namespace CNES {

QPPU::
QPPU(QMachine *qmachine) :
 PPU(qmachine), qmachine_(qmachine)
{
  setObjectName("PPU");

  setFocusPolicy(Qt::StrongFocus);

  setScale(4);

  timer_ = new QTimer;

  connect(timer_, SIGNAL(timeout()), this, SLOT(drawLineSlot()));

  timer_->start(1000/60); // 60Htz
}

void
QPPU::
setScale(int scale)
{
  if (scale != scale_) {
    scale_ = scale;

    updateImage_ = true;
  }
}

void
QPPU::
setMargin(int margin)
{
  if (margin != margin_) {
    margin_ = margin;

    updateImage_ = true;
  }
}

void
QPPU::
drawLineSlot()
{
  updateImage();

  if (lineNum_ >= s_vsyncLines + s_vblank1Lines && lineNum_ < s_numLines - s_vblank2Lines) {
    for (int i = 0; i < 8; ++i)
      drawLine(lineNum_++);
  }
  else {
    drawLine(lineNum_++);
  }

  if (lineNum_ >= s_numLines)
    lineNum_ = 0;

  update();
}

void
QPPU::
updateImage()
{
  if (updateImage_) {
    updateImage_ = false;

    delete ipainter_;
    delete image_;

    // Screen: 32 x 32 (plus margins)
    int xm = scale()*margin();
    int ym = scale()*margin();

    int w = scale()*s_numLines;
    int h = scale()*s_numPixels;

    image_ = new QImage(w + 2*xm, h + 2*ym, QImage::Format_ARGB32);

    image_->fill(0);

    ipainter_ = new QPainter(image_);
  }
}

void
QPPU::
resizeEvent(QResizeEvent *)
{
}

void
QPPU::
paintEvent(QPaintEvent *)
{
  updateImage();

  //---

  QPainter painter(this);

  painter.fillRect(rect(), Qt::black);

  if (image_)
    painter.drawImage(0, 0, *image_);

  painter.setPen(QColor(0,255,0));

  int py = lineNum_*scale();

  painter.drawLine(0, py, width() - 1, py);
}

void
QPPU::
keyPressEvent(QKeyEvent *e)
{
  keyPressed_[e->key()] = true;
}

void
QPPU::
keyReleaseEvent(QKeyEvent *e)
{
  keyPressed_[e->key()] = false;
}

bool
QPPU::
isKey1(int n)
{
  if (n == 0) {
    auto p = keyPressed_.find(Qt::Key_A);

    if (p != keyPressed_.end())
      return (*p).second;
  }

  return false;
}

bool
QPPU::
isKey2(int /*n*/)
{
  return false;
}

void
QPPU::
setColor(uchar c)
{
  static QColor colors[4] {
    QColor(0, 0, 0), QColor(82, 82, 82), QColor(164, 164, 164), QColor(255, 255, 255)
  };

  ipainter_->setPen(colors[c]);
}

void
QPPU::
drawPixel(int x, int y)
{
  int x1 = (x + margin())*scale();
  int y1 = (y + margin())*scale();

  for (int isy = 0; isy < scale(); ++isy) {
    for (int isx = 0; isx < scale(); ++isx) {
      ipainter_->drawPoint(x1 + isx, y1 + isy);
    }
  }
}

QSize
QPPU::
sizeHint() const
{
  int xm = scale()*margin();
  int ym = scale()*margin();

  int w = scale()*s_numLines;
  int h = scale()*s_numPixels;

  return QSize(w + 2*xm, h + 2*ym);
}

}
