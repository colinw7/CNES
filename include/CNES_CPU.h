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

  // screen
  ushort screenPatternAddr() const { return screenPatternAddr_; }

  bool isScreenVisible() const { return screenVisible_; }

  // sprites
  ushort spritePatternAddr() const { return spritePatternAddr_; }
  ushort spritePatternAltAddr() const { return spritePatternAltAddr_; }

  bool isSpriteDoubleHeight() const { return spriteDoubleHeight_; }
  bool isSpritesVisible() const { return spritesVisible_; }

  uchar spriteSize() const { return (isSpriteDoubleHeight() ? 16 : 8); }

  uchar spriteAddr() const { return spriteAddr_; }
  void setSpriteAddr(uchar a) { spriteAddr_ = a; }

  uchar spriteMem(uchar i) const { return spriteMem_[i]; }

  // ppu
  bool isVerticalWrite() const { return verticalWrite_; }

  // name table
  uchar nameTable() const { return nameTable_; }
  ushort nameTableAddr() const { return nameTableAddr_; }

  // interrupts
  bool isSpriteInterrupt() const { return spriteInterrupt_; }
  bool isBlankInterrupt () const { return blankInterrupt_ ; }

  // color
  bool isGrayScale() const { return grayScale_; }

  bool isEmphasizeRed  () const { return emphasizeRed_; }
  bool isEmphasizeGreen() const { return emphasizeGreen_; }
  bool isEmphasizeBlue () const { return emphasizeBlue_; }

  // mask
  bool isSpriteMasked() const { return spriteMask_; }
  bool isImageMasked() const { return imageMask_; }

  //---

  uchar scrollH() const { return scrollHV_[0]; }
  uchar scrollV() const { return scrollHV_[1]; }

  void resetScroll() { scrollHV_[0] = 0; scrollHV_[1] = 0; }

  //---

  virtual void signalNesChanged() { }

 private:
  Machine*       machine_              { nullptr };

  // debug
  bool           debugRead_            { false };
  bool           debugWrite_           { false };

  // screen
  mutable ushort screenPatternAddr_    { 0x0000 };
  bool           screenVisible_        { false };

  // sprites
  mutable ushort spritePatternAddr_    { 0x0000 };
  mutable ushort spritePatternAltAddr_ { 0x1000 };
  bool           spriteDoubleHeight_   { false };
  bool           spritesVisible_       { false };
  mutable uchar  spriteAddr_ { 0 };
  uchar          spriteMem_[256]; // TODO: typically located at $0200-$02FF

  mutable uchar  byteId_               { 0 }; // byte id for PPUSCROLL and PPUADDR

  // ppu (PPUADDR)
  mutable ushort ppuAddr_              { 0x0000 };
  mutable uchar  ppuBuffer_            { 0x00 };
  uchar          ppuAddrHL_[2];
  bool           verticalWrite_        { false }; // for ppu addr increment

  // name table
  uchar          nameTable_            { 0x00 };
  ushort         nameTableAddr_        { 0x0000 };

  // interrupts
  bool           spriteInterrupt_      { false };
  bool           blankInterrupt_       { false };

  // colors
  bool           grayScale_            { false };
  bool           emphasizeRed_         { false };
  bool           emphasizeGreen_       { false };
  bool           emphasizeBlue_        { false };

  // mask
  bool           imageMask_            { false };
  bool           spriteMask_           { false };

  // scroll (PPUSCROLL)
  uchar          scrollHV_[2];

  // keys
  mutable uchar keyNum1_ { 0 };
  mutable uchar keyNum2_ { 0 };

  mutable bool in_ppu_ { false };
};

}

#endif
