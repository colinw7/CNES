#include <CQNES_Cartridge.h>
#include <CQNES_Machine.h>

#include <QPainter>
#include <cmath>

namespace CNES {

QCartridge::
QCartridge(QMachine *qmachine) :
 Cartridge(qmachine), qmachine_(qmachine)
{
  setObjectName("Cartridge");

  updateState();
}

void
QCartridge::
updateState()
{
  int nt = numTiles();

  if (nt > 1) {
    ntx_ = (int) std::round(std::sqrt(nt));
    nty_ = (nt + ntx_ - 1)/ntx_;
  }
  else {
    ntx_ = 1;
    nty_ = 1;
  }
}

void
QCartridge::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.fillRect(rect(), Qt::black);

  painter_ = &painter;

  updateState();

  drawTiles();
}

void
QCartridge::
setColor(uchar c)
{
#if 0
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
#endif
  static QColor colors[64] {
    QColor(0, 0, 0), QColor(85, 85, 85), QColor(170, 170, 170), QColor(255, 255, 255),
  };

  painter_->setPen(colors[c & 0x03]);
}

void
QCartridge::
drawPixel(int x, int y)
{
  ushort tileWidth  = 16*8;
  ushort tileHeight = 16*8;

  int itx = currentTile_ % ntx_;
  int ity = currentTile_ / ntx_;

  int x1 = (x + itx*tileWidth )*scale_ + (itx + 1)*border_;
  int y1 = (y + ity*tileHeight)*scale_ + (ity + 1)*border_;

  for (int isy = 0; isy < scale_; ++isy) {
    for (int isx = 0; isx < scale_; ++isx) {
      painter_->drawPoint(x1 + isx, y1 + isy);
    }
  }
}

QSize
QCartridge::
sizeHint() const
{
  return QSize(ntx_*16*8*scale_ + (ntx_ + 1)*border_, nty_*16*8*scale_ + (nty_ + 1)*2*border_);
}

}
