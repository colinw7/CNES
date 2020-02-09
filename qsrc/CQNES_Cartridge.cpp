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
  static QColor colors[4] {
    QColor(0, 0, 0), QColor(82, 82, 82), QColor(164, 164, 164), QColor(255, 255, 255)
  };

  painter_->setPen(colors[c]);
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
