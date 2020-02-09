#ifndef CQNES_Cartridge_H
#define CQNES_Cartridge_H

#include <CNES_Cartridge.h>
#include <QWidget>

class QPainter;

namespace CNES {

class QMachine;

class QCartridge : public QWidget, public Cartridge {
  Q_OBJECT

 public:
  QCartridge(QMachine *qmachine);

  void updateState() override;

  void paintEvent(QPaintEvent *) override;

  void setColor(uchar c) override;
  void drawPixel(int x, int y) override;

  QSize sizeHint() const override;

 private:
  QMachine *qmachine_ { nullptr };
  QPainter *painter_  { nullptr };
  int       scale_    { 4 };
  int       border_   { 8 };
  int       ntx_      { 1 };
  int       nty_      { 1 };
};

}

#endif
