#ifndef CNES_PPU_H
#define CNES_PPU_H

#include <CNES_Types.h>
#include <vector>

namespace CNES {

class Machine;

class PPU {
 public:
  PPU(Machine *machine);

  virtual ~PPU();

  bool isDebugRead() const { return debugRead_; }
  void setDebugRead(bool b) { debugRead_ = b; }

  bool isDebugWrite() const { return debugWrite_; }
  void setDebugWrite(bool b) { debugWrite_ = b; }

  uchar getByte(ushort addr) const;
  void setByte(ushort addr, uchar c);

  uchar getVRAMByte(ushort addr) const;

  virtual void memChanged(ushort /*addr*/, ushort /*len*/) { }

  //---

  void tick(uchar n);

  //---

  bool isVBlank() const { return vblank_; }
  void setVBlank(bool b) { vblank_ = b; }

  bool isSpriteHit() const { return spriteHit_; }
  void setSpriteHit(bool b) { spriteHit_ = b; }

  virtual bool isKey1(int /*n*/) { return false; }
  virtual bool isKey2(int /*n*/) { return false; }

  void drawLines();
  void drawLine(int y);

  void drawCharLine(int x, int y, uchar c, uchar ac, uchar iby, uchar ix1, uchar ix2);

  void drawSpritesOnLine(int y);
  void drawSpriteLine(int i, int y);
  void drawSpriteCharLine(int x, int y, uchar c, int iby, uchar ac);

  uchar calcNameTableTile (int iy, int ix) const;
  uchar calcAttrTableColor(int iy, int ix) const;

  uchar palette(uchar c) const;
  uchar spritePalette(uchar c) const;

  void drawColorPixel(int x, int y, uchar color);

  virtual void setColor(uchar /*c*/) { }
  virtual void drawPixel(int /*x*/, int /*y*/) { }

  bool isSprite0Hit(int y) const;

 protected:
  using Pixels = std::vector<ushort>;

  static const int s_vsyncLines    { 3 };
  static const int s_vblank1Lines  { 14 };
  static const int s_visibleLines  { 240 }; // NTSC: 224, PAL 240
  static const int s_visibleGap    { 8 }; // NTSC: 224 (32 gap), PAL 240 (8 gap)
  static const int s_vblank2Lines  { 5 };
  static const int s_topMargin     { s_vsyncLines + s_vblank1Lines };
  static const int s_numLines      { s_topMargin + s_visibleLines + + s_vblank2Lines};
  static const int s_visiblePixels { 256 };
  static const int s_numPixels     { 341 };
  static const int s_vblankLine    { s_topMargin + s_visibleLines };

  static const int s_cpuSpeed     { 1'790'000 };
  static const int s_displaySpeed { 60 };
  static const int s_ticksPerLine { s_cpuSpeed/(s_displaySpeed*s_numLines) };

  static const int s_leftMargin  = (s_numPixels - s_visiblePixels)/2;
  static const int s_rightMargin = s_numPixels - s_visiblePixels - s_leftMargin;
  static const int s_rightPixel  = s_leftMargin + s_visiblePixels;

  Machine* machine_      { nullptr };
  bool     debugRead_    { false };
  bool     debugWrite_   { false };
  uchar*   mem_          { nullptr }; // VRAM
  int      lineNum_      { 0 };
  bool     vblank_       { false };
  bool     spriteHit_    { false };
  int      lineTicks_    { s_ticksPerLine };
  int      numDrawLines_ { 0 };
  uchar    scrollV_      { 0 };
  bool     in_ppu_       { false };
  Pixels   pixels_;
};

}

#endif
