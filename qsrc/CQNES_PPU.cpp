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
    int xm = margin();
    int ym = margin();

    int w = scale()*s_numPixels;
    int h = scale()*s_numLines;

    iw_ = w + 2*xm;
    ih_ = h + 2*ym;

    image_ = new QImage(iw_, ih_, QImage::Format_ARGB32);

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

  //---

  // regions
  painter.setPen(QColor(128,128,128));

  int x = s_leftMargin   *scale();
  int w = s_visiblePixels*scale();
  int y = (s_vsyncLines + s_vblank1Lines)*scale();
  int h = s_visibleLines*scale();

  painter.drawRect(QRect(x, y, w, h));

  //---

  // scan line
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
  Qt::Key key;

  if      (n == 0) key = Qt::Key_A;
  else if (n == 1) key = Qt::Key_B;
  else if (n == 2) key = Qt::Key_Insert; // SELECT
  else if (n == 3) key = Qt::Key_Delete; // START
  else if (n == 4) key = Qt::Key_Up;
  else if (n == 5) key = Qt::Key_Down;
  else if (n == 6) key = Qt::Key_Left;
  else if (n == 7) key = Qt::Key_Right;
  else             return false;

  auto p = keyPressed_.find(key);

  if (p != keyPressed_.end())
    return (*p).second;

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
  int x1 = x*scale() + margin();
  int y1 = y*scale() + margin();

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
  const_cast<QPPU *>(this)->updateImage();

  return QSize(iw_, ih_);
}

}
