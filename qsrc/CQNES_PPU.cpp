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

  //---

  timer_ = new QTimer;

  connect(timer_, SIGNAL(timeout()), this, SLOT(drawLineSlot()));

  timer_->start(s_cycleTime);
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
  if (numDrawLines_ > 0) {
    updateImage();

#if 0
    // scan line
    ipainter_->setPen(Qt::black);

    int py = scale()*lineNum_;

    for (int isy = 0; isy < scale(); ++isy)
      ipainter_->drawLine(0, py + isy, width() - 1, py + isy);
#endif

    for (int i = 0; i < numDrawLines_; ++i) {
      drawLine(lineNum_++);

      if (lineNum_ >= s_numLines)
        lineNum_ = 0;
    }

    numDrawLines_ = 0;

    update();
  }
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

  // TODO: get bg color from PPU
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
  if (isShowScanLine()) {
    painter.setPen(QColor(0,255,0));

    int py = lineNum_*scale();

    for (int isy = 0; isy < scale(); ++isy)
      painter.drawLine(0, py + isy, width() - 1, py + isy);
  }
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
  static QColor colors[64] {
    QColor( 84,  84,  84), QColor(  0,  30, 116), QColor(  8,  16, 144), QColor( 48,   0, 136),
    QColor( 68,   0, 100), QColor( 92,   0,  48), QColor( 84,   4,   0), QColor( 60,  24,   0),
    QColor( 32,  42,   0), QColor(  8,  58,   0), QColor(  0,  64,   0), QColor(  0,  60,   0),
    QColor(  0,  50,  60), QColor(  0,   0,   0), QColor(  0,   0,   0), QColor(  0,   0,   0),

    QColor(152, 150, 152), QColor(  8,  76, 196), QColor( 48,  50, 236), QColor( 92,  30, 228),
    QColor(136,  20, 176), QColor(160,  20, 100), QColor(152,  34,  32), QColor(120,  60,   0),
    QColor( 84,  90,   0), QColor( 40, 114,   0), QColor(  8, 124,   0), QColor(  0, 118,  40),
    QColor(  0, 102, 120), QColor(  0,   0,   0), QColor(  0,   0,   0), QColor(  0,   0,   0),

    QColor(236, 238, 236), QColor( 76, 154, 236), QColor(120, 124, 236), QColor(176,  98, 236),
    QColor(228,  84, 236), QColor(236,  88, 180), QColor(236, 106, 100), QColor(212, 136,  32),
    QColor(160, 170,   0), QColor(116, 196,   0), QColor( 76, 208,  32), QColor( 56, 204, 108),
    QColor( 56, 180, 204), QColor( 60,  60,  60), QColor(  0,   0,   0), QColor(  0,   0,   0),

    QColor(236, 238, 236), QColor(168, 204, 236), QColor(188, 188, 236), QColor(212, 178, 236),
    QColor(236, 174, 236), QColor(236, 174, 212), QColor(236, 180, 176), QColor(228, 196, 144),
    QColor(204, 210, 120), QColor(180, 222, 120), QColor(168, 226, 144), QColor(152, 226, 180),
    QColor(160, 214, 228), QColor(160, 162, 160), QColor(  0,   0,   0), QColor(  0,   0,   0),
  };

  ipainter_->setPen(colors[c & 0x3F]);
}

void
QPPU::
drawPixel(int x, int y)
{
  int s = scale();

  int x1 = x*s + margin();
  int y1 = y*s + margin();

  for (int isy = 0; isy < s; ++isy) {
    for (int isx = 0; isx < s; ++isx) {
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
