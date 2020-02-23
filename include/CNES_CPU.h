#ifndef CNES_CPU_H
#define CNES_CPU_H

#include <CNES_Types.h>
#include <C6502.h>

namespace CNES {

class Machine;

class CPU : public C6502 {
 public:
  CPU(Machine *machine);
 ~CPU();

  bool isDebugRead() const { return debugRead_; }
  void setDebugRead(bool b) { debugRead_ = b; }

  bool isDebugWrite() const { return debugWrite_; }
  void setDebugWrite(bool b) { debugWrite_ = b; }

  uchar ppuGetByte(ushort addr) const;

  uchar getByte(ushort addr) const override;
  void setByte(ushort addr, uchar c) override;

  void memset(ushort addr, const uchar *data, ushort len) override;

  void tick(uchar n) override;

  bool isScreen(ushort pos, ushort len) const override;

  //---

  virtual void signalNesChanged() { }

 private:
  Machine*      machine_    { nullptr };

  // debug
  bool          debugRead_  { false };
  bool          debugWrite_ { false };

  // keys
  mutable uchar keyNum1_ { 0 };
  mutable uchar keyNum2_ { 0 };

  mutable bool in_ppu_ { false };
};

}

#endif
