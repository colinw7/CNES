#ifndef CQNES_CPU_H
#define CQNES_CPU_H

#include <CNES_CPU.h>
#include <QObject>

namespace CNES {

class QMachine;

class QCPU : public QObject, public CPU {
  Q_OBJECT

 public:
  QCPU(QMachine *machine);

//void registerChanged(Reg) override { emit registerChangedSignal(); }

//void flagsChanged() override { emit flagsChangedSignal(); }
//void stackChanged() override { emit stackChangedSignal(); }
  void pcChanged   () override { emit pcChangedSignal   (); }

  void memChanged(ushort addr, ushort len) override { emit memChangedSignal(addr, len); }

  void handleBreak  () override { emit handleBreakSignal(); }
  void breakpointHit() override { emit breakpointHitSignal(); }
  void illegalJump  () override { emit illegalJumpSignal(); }
  void handleNMI    () override { emit nmiSignal(); }

  void breakpointsChanged() override { emit breakpointsChangedSignal(); }

  void signalNesChanged() override { emit nesChangedSignal(); }

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

  void nesChangedSignal();
};

}

#endif
