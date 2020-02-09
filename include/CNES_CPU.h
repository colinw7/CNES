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

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  uchar ppuGetByte(ushort addr) const;

  uchar getByte(ushort addr) const override;
  void setByte(ushort addr, uchar c) override;

  void memset(ushort addr, const uchar *data, ushort len) override;

  void tick(uchar n) override;

  bool isScreen(ushort pos, ushort len) const override;

  bool isBlankInterrupt() const { return blankInterrupt_; }

  ushort nameTableAddr() const { return nameTableAddr_; }

  ushort screenPatternAddr() const { return screenPatternAddr_; }

  bool isScreenVisible() const { return screenVisible_; }

 private:
  Machine*       machine_            { nullptr };
  bool           debug_              { false };
  mutable ushort spritePatternAddr_  { 0 };
  mutable ushort screenPatternAddr_  { 0x0000 };
  mutable ushort ppuAddr_            { 0 };
  ushort         nameTableAddr_      { 0 };
  bool           verticalWrite_      { false };
  bool           spriteDoubleHeight_ { false };
  bool           hitInterrupt_       { false };
  bool           blankInterrupt_     { false };
  bool           imageMask_          { false };
  bool           spriteMask_         { false };
  bool           screenVisible_      { false };
  bool           spritesSwitch_      { false };
  uchar          scrollXY_[2];
  uchar          scrollId_           { 0 };
  uchar          ppuAddrHL_[2];
  uchar          ppuId_              { 0 };

  mutable uchar  spriteAddr_ { 0 };
  uchar          spriteMem_[256];

  mutable uchar keyNum1_ { 0 };
  mutable uchar keyNum2_ { 0 };

  mutable bool in_ppu_ { false };
};

}

#endif
