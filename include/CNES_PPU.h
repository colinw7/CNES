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

  uchar getControlByte(ushort addr) const;
  void setControlByte(ushort addr, uchar c);

  void copySpriteMem(uchar c);

  virtual void memChanged(ushort /*addr*/, ushort /*len*/) { }

  virtual void spritesChanged() { }

  //---

  void tick(uchar n);

  //---

  // screen
  ushort screenPatternAddr() const { return screenPatternAddr_; }

  bool isScreenVisible() const { return screenVisible_; }

  // name table
  uchar nameTable() const { return nameTable_; }
  ushort nameTableAddr() const { return nameTableAddr_; }

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

  bool isVBlank() const { return vblank_; }
  void setVBlank(bool b) { vblank_ = b; }

  bool isSpriteHit() const { return spriteHit_; }
  void setSpriteHit(bool b) { spriteHit_ = b; }

  virtual bool isKey1(int /*n*/) { return false; }
  virtual bool isKey2(int /*n*/) { return false; }

  void drawLines();
  void drawLine(int y);

  void drawCharLine(int x, int y, uchar c, uchar ac, uchar iby, uchar ix1, uchar ix2);

  //---

  struct SpriteData {
    int   num     { 0 };
    uchar pattern { 0 };
    uchar bank    { 0 };
    uchar color   { 0 };
    bool  behind  { false };
    bool  flipX   { false };
    bool  flipY   { false };
    bool  custom  { false };
    int   x       { 0 };
    int   y       { 0 };
  };

  void drawSpritesOnLine(int y);
  void drawSpriteLine(int i, int y);
  void drawSpriteCharLine(int x, int y, uchar iby, const SpriteData &spriteData);

  void drawSpriteAt(int i, int x, int y, int o);

  //---

  uchar calcNameTableTile (int iy, int ix) const;
  uchar calcAttrTableColor(int iy, int ix) const;

  uchar palette(uchar c) const;
  uchar spritePalette(uchar c) const;

  void getSpriteData(int spriteNum, SpriteData &spriteData) const;

  void drawLinePixel       (int x, int y, uchar color);
  void drawColorPixel      (int x, int y, uchar color);
  void drawCustomColorPixel(int x, int y, uchar color);

  virtual void setColor(uchar /*c*/) { }
  virtual void drawPixel(int /*x*/, int /*y*/) { }

//bool isSprite0Hit(int y) const;

 protected:
  using SPixels = std::vector<ushort>;
  using Pixels  = std::vector<uchar>;

  static const int s_vsyncLines    { 3 };
  static const int s_vblank1Lines  { 14 };
  static const int s_visibleLines  { 240 }; // NTSC: 224, PAL 240
  static const int s_vChars        { 30 };
  static const int s_visibleGap    { 8 }; // NTSC: 224 (32 gap), PAL 240 (8 gap)
  static const int s_vblank2Lines  { 5 };
  static const int s_topMargin     { s_vsyncLines + s_vblank1Lines };
  static const int s_numLines      { s_topMargin + s_visibleLines + + s_vblank2Lines};
  static const int s_visiblePixels { 256 };
  static const int s_hChars        { 32 };
  static const int s_numPixels     { 341 };
  static const int s_vblankLine    { s_topMargin + s_visibleLines };

  static const int s_cpuSpeed     { 1'790'000 };
  static const int s_displaySpeed { 60 };
  static const int s_ticksPerLine { s_cpuSpeed/(s_displaySpeed*s_numLines) };

  static const int s_leftMargin  = (s_numPixels - s_visiblePixels)/2;
  static const int s_rightMargin = s_numPixels - s_visiblePixels - s_leftMargin;
  static const int s_rightPixel  = s_leftMargin + s_visiblePixels;

  //---

  Machine* machine_ { nullptr };

  // debug
  bool debugRead_  { false };
  bool debugWrite_ { false };

  // ppu memory
  uchar* mem_ { nullptr }; // VRAM

  //---

  // screen
  mutable ushort screenPatternAddr_    { 0x0000 };
  bool           screenVisible_        { false };

  // sprites
  mutable ushort spritePatternAddr_    { 0x0000 };
  mutable ushort spritePatternAltAddr_ { 0x1000 };
  bool           spriteDoubleHeight_   { false };
  bool           spritesVisible_       { false };
  mutable uchar  spriteAddr_           { 0 };
  uchar          spriteMem_[256];      // TODO: typically located at $0200-$02FF

  // name table
  uchar          nameTable_            { 0x00 };
  ushort         nameTableAddr_        { 0x0000 };

  // ppu memory (PPUADDR)
  mutable ushort ppuAddr_              { 0x0000 };
  mutable uchar  ppuBuffer_            { 0x00 };
  uchar          ppuVal_               { 0x00 };
  uchar          ppuAddrHL_[2];
  bool           verticalWrite_        { false }; // for ppu addr increment

  // scroll (PPUSCROLL)
  uchar          scrollHV_[2];                 // scroll h/v values
  uchar          scrollV_              { 0 };  // copy of scroll v for draw lines

  mutable uchar  byteId_               { 0 }; // byte id for PPUSCROLL and PPUADDR

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

  //---

  // draw state
  int      scanLineNum_     { 0 };     // scan line number
  int      pixelLineNum_    { 0 };     // pixel line number (0 - 240)
  bool     vblank_          { false };
  bool     spriteHit_       { false };
  bool     spritesOverflow_ { false };
  bool     in_ppu_          { false };
  uchar    color0_          { 0 };
  SPixels  screenPixels_;
  Pixels   linePixels_;

  // draw timing
  int      lineTicks_       { s_ticksPerLine }; // ticks to wait before drawing a line
  int      numDrawLines_    { 0 };              // lines to draw on next paint
};

}

#endif
