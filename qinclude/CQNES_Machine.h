#ifndef CQNES_MACHINE_H
#define CQNES_MACHINE_H

#include <CNES_Machine.h>
#include <QObject>

namespace CNES {

class QCPU;
class QPPU;
class QCartridge;

class QMachine : public QObject, public Machine {
  Q_OBJECT

 public:
  QMachine();

  void init() override;

  QCPU       *getQCPU () const { return qcpu_ ; }
  QPPU       *getQPPU () const { return qppu_ ; }
  QCartridge *getQCart() const { return qcart_; }

 signals:
//void registerChangedSignal();
//void flagsChangedSignal();
//void stackChangedSignal();
  void pcChangedSignal();
  void memChangedSignal(ushort addr, ushort len);

  void handleBreakSignal();
  void breakpointHitSignal();
  void illegalJumpSignal();
  void nmiSignal();

  void breakpointsChangedSignal();

 private:
  QCPU       *qcpu_  { nullptr };
  QPPU       *qppu_  { nullptr };
  QCartridge *qcart_ { nullptr };
};

}

#endif
