#ifndef CNES_PPU_H
#define CNES_PPU_H

#include <CNES_Types.h>

namespace CNES {

class Machine;

class PPU {
 public:
  PPU(Machine *machine);

  virtual ~PPU();

  uchar getByte(ushort addr) const;

  void setByte(ushort addr, uchar c);

  bool isVBlank() const { return vblank_; }
  void setVBlank(bool b) { vblank_ = b; }

  bool isSpriteHit() const { return spriteHit_; }
  void setSpriteHit(bool b) { spriteHit_ = b; }

  virtual bool isKey1(int /*n*/) { return false; }
  virtual bool isKey2(int /*n*/) { return false; }

  void drawLines();
  void drawLine(int y);

  void drawNameTable();

  void drawChar(int x, int y, uchar c, uchar ac);

  void drawCharLine(int x, int y, uchar c, uchar ac);

  virtual void fillScreen(uchar /*c*/) { }

  virtual void setColor(uchar /*c*/) { }

  virtual void drawPixel(int /*x*/, int /*y*/) { }

 protected:
  static const int s_numLines      { 262 };
  static const int s_vsyncLines    { 3 };
  static const int s_vblank1Lines  { 14 };
  static const int s_visibleLines  { 240 };
  static const int s_vblank2Lines  { 5 };
  static const int s_numPixels     { 341 };
  static const int s_visiblePixels { 256 };

  Machine* machine_   { nullptr };
  uchar*   mem_       { nullptr }; // VRAM
  int      lineNum_   { 0 };
  bool     vblank_    { false };
  bool     spriteHit_ { false };
};

}

#endif
