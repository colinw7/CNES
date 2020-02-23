#include <CNES_PPU.h>
#include <CNES_Machine.h>
#include <CNES_CPU.h>
#include <CNES_Cartridge.h>
#include <cassert>

namespace CNES {

PPU::
PPU(Machine *machine) :
 machine_(machine)
{
  assert(machine_);

  // init ppu memory
  mem_ = new uchar [0x4000]; // 16k

  // init screen pixels
  int np = s_visibleLines*s_visiblePixels;

  screenPixels_.resize(np);

  memset(&screenPixels_[0], 0, np*sizeof(ushort));

  // init line pixels
  linePixels_.resize(s_visiblePixels);

  memset(&linePixels_[0], 0, s_visiblePixels*sizeof(uchar));
}

PPU::
~PPU()
{
  delete mem_;
}

uchar
PPU::
getByte(ushort addr) const
{
  auto returnChar = [&](uchar c) {
    if (isDebugRead() && ! in_ppu_)
      std::cerr << "PPU::getByte " <<
        std::hex << addr << " " << std::hex << int(c) << "\n";
    return c;
  };

  // With vertical mirroring, tables 2 and 3 are the mirrors of pages 0 and 1 appropriately.
  // With horizontal mirroring, pages 1 and 3 are the mirrors of pages 0 and 2 appropriately.

  // Pattern Table 0 (256x2x8, may be VROM)
  if      (addr < 0x1000) {
    uchar c = getVRAMByte(addr);

    return returnChar(c);
  }
  // Pattern Table 1 (256x2x8, may be VROM)
  else if (addr < 0x2000) {
    uchar c = getVRAMByte(addr);

    return returnChar(c);
  }
  // Name Table 0 (32x25 tiles)
  else if (addr < 0x23C0) {
  }
  // Attribute Table 0
  else if (addr < 0x2400) {
  }
  // Name Table 1 (32x25 tiles)
  else if (addr < 0x27C0) {
  }
  // Attribute Table 1
  else if (addr < 0x2800) {
  }
  // Name Table 2 (32x25 tiles)
  else if (addr < 0x2BC0) {
    auto *cpu = machine_->getCPU();

    if (! cpu->isDebugger())
      std::cerr << "PPU::getByte (Name Table 2) " << std::hex << addr << "\n";
  }
  // Attribute Table 2
  else if (addr < 0x2C00) {
  }
  // Name Table 3 (32x25 tiles)
  else if (addr < 0x2FC0) {
    auto *cpu = machine_->getCPU();

    if (! cpu->isDebugger())
      std::cerr << "PPU::getByte (Name Table 3) " << std::hex << addr << "\n";
  }
  // Attribute Table 3
  else if (addr < 0x3000) {
  }
  // Empty
  else if (addr < 0x3F00) {
  }
  // Image Palette
  else if (addr < 0x3F10) {
  }
  // Sprite Palette
  else if (addr < 0x3F20) {
    // The $3F00 and $3F10 locations in VRAM mirror each other (i.e. it
    // is the same memory cell) and define the background color of the picture.
    if (addr == 0x3F10)
      addr = 0x3F00;
  }
  // Empty
  else if (addr < 0x4000) {
  }

  uchar c = mem_[addr & 0x3FFF];

  return returnChar(c);
}

uchar
PPU::
getVRAMByte(ushort addr) const
{
  auto *cart = machine_->getCart();

  uchar c;

  if (! cart->getVRAMByte(addr, c))
    c = mem_[addr & 0x3FFF];

  return c;
}

uchar
PPU::
getControlByte(ushort addr) const
{
  auto *cpu = machine_->getCPU();

  uchar c = 0x00;

  // PPU Control Register 1 (PPUCTRL)
  if      (addr == 0x2000) {
    if (! cpu->isDebugger())
      std::cerr << "Read PPUCTRL Unhandled\n";
  }
  // PPU Control Register 2 (PPUMASK)
  else if (addr == 0x2001) {
    if (! cpu->isDebugger())
      std::cerr << "Read PPUMASK Unhandled\n";
  }
  // PPU Status Register (PPUSTATUS)
  // (read only)
  else if (addr == 0x2002) {
    // Init width least significant bits previously written into a PPU register
    c = ppuVal_ & 0x1F;

    // TODO: Sprite overflow on bit 5

    if (spritesOverflow_) c |= 0x20;
    if (isSpriteHit   ()) c |= 0x40;
    if (isVBlank      ()) c |= 0x80;

    // reset on read
    if (! cpu->isDebugger()) {
      auto *th = const_cast<PPU *>(this);

    //th->setSpriteHit(false);
      th->setVBlank   (false);

      byteId_ = 0;
    }
  }
  // Sprite Memory Address (OAMADDR)
  else if (addr == 0x2003) {
    if (! cpu->isDebugger())
      std::cerr << "Read OAMADDR Unhandled\n";
  }
  // Sprite Memory Data (OAMDATA)
  else if (addr == 0x2004) {
    if (! cpu->isDebugger()) {
      c = spriteMem_[spriteAddr_++];

      if (isDebugRead() && ! in_ppu_)
        std::cerr << "CPU::getSpriteByte " <<
          std::hex << addr << " " << std::hex << int(c) << "\n";
    }
    else {
      c = spriteMem_[spriteAddr_];
    }
  }
  // Background Scroll (PPUSCROLL)
  else if (addr == 0x2005) {
    if (! cpu->isDebugger())
      std::cerr << "Read PPUSCROLL Unhandled\n";
  }
  // PPU Memory Address (PPUADDR)
  else if (addr == 0x2006) {
    if (! cpu->isDebugger())
      std::cerr << "Read PPUADDR Unhandled\n";
  }
  // PPU Memory Data (PPUDATA)
  else if (addr == 0x2007) {
    if (! cpu->isDebugger()) {
      if (ppuAddr_ < 0x3F00) {
        c = ppuBuffer_;

        ppuBuffer_ = getByte(ppuAddr_);
      }
      else {
        c = getByte(ppuAddr_);

        ppuBuffer_ = getByte(ppuAddr_); // TODO: mirrored nametable data ?
      }

      ++ppuAddr_;
    }
    else
      c = ppuBuffer_;
  }
  else {
    if (! cpu->isDebugger())
      std::cerr << "Read PPU Unhandled\n";
  }

  return c;
}

void
PPU::
setControlByte(ushort addr, uchar c)
{
  // PPU Control Register 1 (PPUCTRL)
  if      (addr == 0x2000) {
    // c & 0x01 : Add 256 to the X scroll position
    // c & 0x02 : Add 240 to the Y scroll position
    nameTable_     = (c & 0x03);                // TODO: mirroring ?
    nameTableAddr_ = 0x2000 + nameTable_*0x400; // TODO: mirroring ?

    verticalWrite_      = (c & 0x04) >> 2;
    spritePatternAddr_  = (c & 0x08 ? 0x1000 : 0x0000);
    screenPatternAddr_  = (c & 0x10 ? 0x1000 : 0x0000);
    spriteDoubleHeight_ = (c & 0x20) >> 5;
//  spriteInterrupt_    = (c & 0x40) >> 6; // TODO: wrong
//  ppuMasterSlave_     = (c & 0x40) >> 6;
    blankInterrupt_     = (c & 0x80) >> 7;

    spritePatternAltAddr_ = (spritePatternAddr_ == 0x1000 ? 0x0000 : 0x1000);
  }
  // PPU Control Register 2 (PPUMASK)
  else if (addr == 0x2001) {
    grayScale_      = (c & 0x01);
    imageMask_      = (c & 0x02) >> 1;
    spriteMask_     = (c & 0x04) >> 2;
    screenVisible_  = (c & 0x08) >> 3;
    spritesVisible_ = (c & 0x10) >> 4;
    emphasizeRed_   = (c & 0x20) >> 5;
    emphasizeGreen_ = (c & 0x40) >> 6;
    emphasizeBlue_  = (c & 0x80) >> 7;
  }
  // PPU Status Register (PPUSTATUS)
  else if (addr == 0x2002) {
    std::cerr << "Write PPUSTATUS Unhandled\n";
  }
  // Sprite Memory Address (OAMADDR)
  else if (addr == 0x2003) {
    setSpriteAddr(c);
  }
  // Sprite Memory Data (OAMDATA)
  else if (addr == 0x2004) {
    spriteMem_[spriteAddr_++] = c;

    spritesChanged();
  }
  // Background Scroll (PPUSCROLL)
  // TODO: shares internal register with PPUADDR ?
  else if (addr == 0x2005) {
    // ignore vertical scroll value if >= 240
    if (byteId_ != 1 || c < 0xF0)
      scrollHV_[byteId_] = c;

    byteId_ = 1 - byteId_;
  }
  // PPU Memory Address (PPUADDR)
  else if (addr == 0x2006) {
    ppuVal_ = c;

    ppuAddrHL_[byteId_] = ppuVal_;

    byteId_ = 1 - byteId_;

    ppuAddr_ = (ppuAddrHL_[0] << 8) | ppuAddrHL_[1];
    ppuAddr_ &= 0x3FFF;
  }
  // PPU Memory Data (PPUDATA)
  else if (addr == 0x2007) {
    setByte(ppuAddr_, c);

    if (verticalWrite_)
      ppuAddr_ += s_hChars;
    else
      ++ppuAddr_;
  }
  else {
    std::cerr << "Write PPU Unhandled\n";
  }
}

void
PPU::
setByte(ushort addr, uchar c)
{
  auto *cpu = machine_->getCPU();

  if (isDebugWrite() && ! cpu->isDebugger())
    std::cerr << "PPU::setByte " <<
          std::hex << addr << " " << std::hex << int(c) << "\n";

  // Pattern Table 0 (256x2x8, may be VROM)
  if      (addr < 0x1000) {
    mem_[addr & 0x3FFF] = c;
  }
  // Pattern Table 1 (256x2x8, may be VROM)
  else if (addr < 0x2000) {
    mem_[addr & 0x3FFF] = c;
  }
  else {
    // The $3F00 and $3F10 locations in VRAM mirror each other (i.e. it
    // is the same memory cell) and define the background color of the picture.
    if (addr == 0x3F10)
      addr = 0x3F00;

    mem_[addr & 0x3FFF] = c;
  }

  memChanged(addr, 1);
}

void
PPU::
copySpriteMem(uchar c)
{
  auto *cpu = machine_->getCPU();

  cpu->memget(c << 8, &spriteMem_[0], 0x100);

  spritesChanged();
}

void
PPU::
tick(uchar n)
{
  // display runs at 60hz (NTSC), 50hz (PAL)
  // cpu @ 1.79Mhz (NTSC), 1.66Mhz (PAL)

  static const int ticksPerLine = 1'790'000/60/s_numLines;

  lineTicks_ -= n;

  if (lineTicks_ <= 0) {
    ++numDrawLines_;

    lineTicks_ = ticksPerLine;
  }
}

void
PPU::
drawLines()
{
  for (int y = 0; y < s_numLines; ++y)
    drawLine(y);
}

#if 0
void
PPU::
drawNameTable()
{
  ushort nameTableAddr = nameTableAddr(); // TODO: mirroring

  //---

  uchar bg = palette(0);

  fillScreen(bg);

  //---

  // TODO: include Attribute Table colors

  // name table
  for (int iy = 0; iy < 32; ++iy) {
    int y = iy*8;

    for (int ix = 0; ix < s_hChars; ++ix, ++nameTableAddr) {
      int x = ix*8;

      //---

      // get tile number and attribute color
      uchar c  = calcNameTableTile (iy, ix);
      uchar ac = calcAttrTableColor(iy, ix);

      //---

      drawChar(x, y, c, ac);
    }
  }
}
#endif

void
PPU::
drawLine(int y)
{
  in_ppu_ = true;

  scanLineNum_  = y;
  pixelLineNum_ = scanLineNum_ - s_topMargin;

#if 0
  if (! spriteHit_) {
    if (isSprite0Hit(scanLineNum_)) {
      spriteHit_ = true;

      if (isSpriteInterrupt()) {
        auto *cpu = machine_->getCPU();

        if (! cpu->isHalt() && ! cpu->inNMI())
          cpu->resetNMI();
      }
    }
  }
#endif

  // vertical sync
  if      (scanLineNum_ < s_vsyncLines) {
  }
  // vblank 1
  else if (scanLineNum_ < s_topMargin) {
  }
  // screen
  else if (scanLineNum_ < s_topMargin + s_visibleLines) {
    // NOTE: sprite evaluation starts at line 65 (3 + 14 + 48 ?)
    vblank_ = false;

    // reset line pixels to bg
    color0_ = palette(0);

    memset(&linePixels_[0], color0_, s_visiblePixels*sizeof(uchar));

    //---

    int sy  = scrollV_;   // scroll vertical
    int syb = sy/8;       // scroll vertical bytes
    int syo = sy - syb*8; // scroll vertical byte pixel offset

    int iy  = pixelLineNum_/8;      // vertical byte index in associated char data
    int iby = pixelLineNum_ - iy*8; // sub pixel in vertical byte

    int iy1  = iy - syb;  // adjusted vertical byte index in associated char data
    int iby1 = iby - syo; // adjusted sub pixel in vertical byte

    if (iby1 < 0) {
      --iy1;

      iby1 = 8 + iby1;
    }

    //---

    int x = 0;

    // hblank 1
    for (int ix = 0; ix < s_leftMargin; ++ix)
      ++x;

    //---

    int x1 = x - s_leftMargin;

    if (isScreenVisible()) {
      int sx   = scrollH();  // scroll horizontal
      int sxb  = sx/8;       // scroll horizontal bytes
      int sxo  = sx - sxb*8; // scroll horizontal byte pixel offset
      int isxo = 8 - sxo;    // inverse of scroll horizontal byte pixel offset

      // draw partial left character
      if (sxo > 0) {
        // get tile number and attribute color
        uchar c  = calcNameTableTile (iy1, -sxb - 1);
        uchar ac = calcAttrTableColor(iy1, -sxb - 1);

        //---

        drawCharLine(x1 - isxo, pixelLineNum_, c, ac, iby1, isxo, 8);
      }

      for (int ix = 0; ix < s_hChars; ++ix, x1 += 8) {
        int ix1 = ix - sxb; // horizontal byte index in associated char data

        //---

        // get tile number and attribute color
        uchar c  = calcNameTableTile (iy1, ix1);
        uchar ac = calcAttrTableColor(iy1, ix1);

        //---

        if (ix == 31 && sxo > 0)
          drawCharLine(x1 + sxo, pixelLineNum_, c, ac, iby1, 0, isxo);
        else {
          drawCharLine(x1 + sxo, pixelLineNum_, c, ac, iby1, 0, 8);
        }
      }
    }
    else {
      // blank line
      for (int ix = 0; ix < 256; ++ix, ++x1) {
        if (imageMask_ && ix < 8)
          continue;

        drawLinePixel(ix, pixelLineNum_, color0_);
      }
    }

    //---

    drawSpritesOnLine(pixelLineNum_);

    //---

    // hblank 2
    x = x1 + s_leftMargin;

    for (int ix = 0; ix < s_rightMargin; ++ix)
      ++x;
  }
  // vblank 2
  else {
    setSpriteAddr(0); // reset

    // NMI on first vblank line (if enabled)
    if (scanLineNum_ == s_vblankLine) {
    //resetScroll();

      //---

      vblank_    = true;
      spriteHit_ = false;

      if (isBlankInterrupt()) {
        auto *cpu = machine_->getCPU();

        if (! cpu->isHalt() && ! cpu->inNMI())
          cpu->resetNMI();
      }
    }

    if (scanLineNum_ == s_numLines - 1) {
      scrollV_ = scrollV();
    }
  }

  in_ppu_ = false;
}

#if 0
void
PPU::
drawChar(int x, int y, uchar c, uchar /*ac*/)
{
  auto *cart = machine_->getCart();

  cart->drawPPUChar(x, y, c);
}
#endif

// draw char line at offset (iby) from top of char at (x, y)
void
PPU::
drawCharLine(int x, int y, uchar c, uchar ac, uchar iby, uchar ix, uchar nx)
{
  int y1 = y - iby; // adjust to top of char

  ushort p = screenPatternAddr() + c*16 + iby;

  uchar c1 = getVRAMByte(p    ); // color bit 0
  uchar c2 = getVRAMByte(p + 8); // color bit 1

  for (int ibx = ix; ibx < nx; ++ibx) {
    bool b1 = (c1 & (1 << (7 - ibx)));
    bool b2 = (c2 & (1 << (7 - ibx)));

    uchar color = palette((b1 | (b2 << 1)) | ac);

    if (imageMask_ && x + ibx < 8)
      continue;

    drawLinePixel(x + ibx, y1 + iby, color);
  }
}

uchar
PPU::
calcNameTableTile(int iy, int ix) const
{
  if (iy < 0) iy += s_vChars;
  if (ix < 0) ix += s_hChars;

  assert(ix >= 0 && iy >= 0);

  // 32 bytes per line in name table (8 pixels high)
  ushort offset = s_hChars*iy + ix;

  assert(offset < 0x03C0);

  ushort nameTableAddr = this->nameTableAddr() + offset;

  return getByte(nameTableAddr); // tile number
}

uchar
PPU::
calcAttrTableColor(int iy, int ix) const
{
  if (iy < 0) iy += s_vChars;
  if (ix < 0) ix += s_hChars;

  assert(ix >= 0 && iy >= 0);

  int iay = iy >> 2;
  int iax = ix >> 2;

  ushort offset = 0x03C0 + iay*8 + iax;

  assert(offset < 0x0400);

  ushort attrTableAddr = this->nameTableAddr() + offset;

  uchar ac = getByte(attrTableAddr);

  uchar ac1 = 0;

  if (! (iy & 0x02)) {
    if (! (ix & 0x02)) ac1 = (ac & 0x03) << 2;
    else               ac1 =  ac & 0x0C;
  }
  else {
    if (! (ix & 0x02)) ac1 = (ac & 0x30) >> 2;
    else               ac1 = (ac & 0xC0) >> 4;
  }

  return ac1;
}

uchar
PPU::
palette(uchar c) const
{
  return getByte(0x3F00 + c);
}

uchar
PPU::
spritePalette(uchar c) const
{
  return getByte(0x3F10 + c);
}

#if 0
void
PPU::
drawSprites()
{
  bool visible = isSpritesVisible();
  if (! visible) return;

  auto *cart = machine_->getCart();

  //---

  for (int i = 0; i < 64; ++i) {
    SpriteData spriteData;

    getSpriteData(i, spriteData);

    int y1 = spriteData.y + 1; // next line

    cart->drawSpriteChar(spriteData.x, y1, spriteData.pattern, spriteData.color,
                         spriteData.flipX, spriteData.flipY);
  }
}
#endif

void
PPU::
drawSpriteAt(int spriteNum, int x, int y, int o)
{
  SpriteData spriteData;

  spriteData.custom = true;

  getSpriteData(spriteNum, spriteData);

  spriteData.pattern += o;

  int pixelLineNum = pixelLineNum_;

  for (uchar iy = 0; iy < 8; ++iy) {
    pixelLineNum_ = y + iy;

    drawSpriteCharLine(x, y + iy, iy, spriteData);
  }

  pixelLineNum_ = pixelLineNum;
}

void
PPU::
drawSpritesOnLine(int y)
{
  bool visible = isSpritesVisible();
  if (! visible) return;

  //---

  uchar spriteHeight = (isSpriteDoubleHeight() ? 16 : 8);

  //---

  using VisibleSprites = std::vector<int>;

  VisibleSprites visibleSprites;

  for (int spriteNum = 0; spriteNum < 64; ++spriteNum) {
    SpriteData spriteData;

    getSpriteData(spriteNum, spriteData);

    int y1 = spriteData.y + 1; // top
    int y2 = y1 + spriteHeight - 1;

    if (y2 < y || y1 > y)
      continue;

    visibleSprites.push_back(spriteNum);
  }

  //----

  spritesOverflow_ = false;

  int ns = visibleSprites.size();

  if (ns > 8) {
    spritesOverflow_ = true;

    ns = 8;
  }

  for (const auto &spriteNum : visibleSprites)
    drawSpriteLine(spriteNum, y);
}

void
PPU::
drawSpriteLine(int spriteNum, int y)
{
  in_ppu_ = true;

  //---

  SpriteData spriteData;

  getSpriteData(spriteNum, spriteData);

  int y1 = spriteData.y + 1; // top

  drawSpriteCharLine(spriteData.x, y, y - y1, spriteData);

  in_ppu_ = false;
}

// draw nth sprite line (iby) for sprite at (x, y) and character (c) and
// bits 2 and 3 (ac)
void
PPU::
drawSpriteCharLine(int x, int y, uchar iby, const SpriteData &spriteData)
{
  bool doubleHeight = (! spriteData.custom ? this->isSpriteDoubleHeight() : false);

  //---

  ushort p;

  if (! doubleHeight) {
    assert(iby < 8);

    int iby1 = (spriteData.flipY ? 7 - iby : iby);

    p = spritePatternAddr() + spriteData.pattern*16 + iby1;
  }
  else {
    assert(iby < 16);

    // top half (sub index 0)
    if (iby < 8) {
      // flip bits and sub index
      if (spriteData.flipY) {
        int iby1 = 7 - iby; // flip bits

        p = spriteData.bank*0x1000 + (spriteData.pattern + 1)*16 + iby1;
      }
      else {
        p = spriteData.bank*0x1000 + spriteData.pattern*16 + iby;
      }
    }
    // bottom half (sub index 1)
    else {
      // flip bits and sub index
      if (spriteData.flipY) {
        int iby1 = 15 - iby; // flip bits and adjust range

        p = spriteData.bank*0x1000 + spriteData.pattern*16 + iby1;
      }
      else {
        int iby1 = iby - 8; // adjust range

        p = spriteData.bank*0x1000 + (spriteData.pattern + 1)*16 + iby1;
      }
    }
  }

  //---

  uchar c1 = getVRAMByte(p    ); // color bit 0
  uchar c2 = getVRAMByte(p + 8); // color bit 1

  for (int ibx = 0; ibx < 8; ++ibx, ++x) {
    if (! spriteData.custom) {
      if (x < 0 || x >= s_visiblePixels)
        continue;

      if (spriteMask_ && x < 8)
        continue;
    }

    // get color bits 0 and 1 from sprite pattern
    int ibx1 = (spriteData.flipX ? ibx : 7 - ibx);

    bool b1 = (c1 & (1 << ibx1));
    bool b2 = (c2 & (1 << ibx1));

    uchar color = spritePalette((b1 | (b2 << 1)) | spriteData.color);

    if (! spriteData.custom) {
      if (color == color0_)
        continue;

      bool hit = (linePixels_[x] != color0_);

      if (hit) {
        if (spriteData.num == 0)
          spriteHit_ = true;

        if (! spriteData.behind)
          drawLinePixel(x, y, color);
      }
      else {
        drawLinePixel(x, y, color);
      }
    }
    else {
      drawCustomColorPixel(x, y, color);
    }
  }
}

void
PPU::
getSpriteData(int spriteNum, SpriteData &spriteData) const
{
  int ispriteAddr = 4*spriteNum;
  assert(ispriteAddr <= 252);

  spriteData.num     = spriteNum;
  spriteData.pattern = spriteMem(ispriteAddr + 1);

  // double height has pattern bank in bit 0. Pattern is even index (mask out bit 0)
  if (! spriteData.custom && isSpriteDoubleHeight()) {
    spriteData.bank = (spriteData.pattern & 1);

    spriteData.pattern &= 0xFE;
  }

  uchar attr = spriteMem(ispriteAddr + 2);

  spriteData.color  = (attr & 0x03) << 2;
  spriteData.behind = attr & 0x20;
  spriteData.flipX  = attr & 0x40;
  spriteData.flipY  = attr & 0x80;

  spriteData.x = spriteMem(ispriteAddr + 3);
  spriteData.y = spriteMem(ispriteAddr + 0);
}

void
PPU::
drawLinePixel(int x, int y, uchar color)
{
  assert(x >= 0 && x < s_visiblePixels);
  assert(y == pixelLineNum_);

  linePixels_[x] = color;

  drawColorPixel(x, y, color);
}

void
PPU::
drawColorPixel(int x, int y, uchar color)
{
  assert(x >= 0 && x < s_visiblePixels);
  assert(y >= 0 && y < s_visibleLines );

  int ind = y*s_visiblePixels + x;

  if (isGrayScale())
    color &= 0x30;

  uchar ec { 0 };

  if (isEmphasizeRed  ()) ec |= 0x01;
  if (isEmphasizeGreen()) ec |= 0x02;
  if (isEmphasizeBlue ()) ec |= 0x04;

  ushort pixel = (ec << 8) | color;

  if (screenPixels_[ind] != pixel) {
    screenPixels_[ind] = pixel;

    //---

    setColor(color);

    drawPixel(x + s_leftMargin, y + s_topMargin);
  }
}

void
PPU::
drawCustomColorPixel(int x, int y, uchar color)
{
  setColor(color);

  drawPixel(x, y);
}

#if 0
bool
PPU::
isSprite0Hit(int y) const
{
  // Sprite 0 hit does not trigger in any area where the background or sprites are hidden

  bool visible = isSpritesVisible();

  if (! visible)
    return false;

  //---

  SpriteData spriteData;

  getSpriteData(spriteNum, spriteData);

  //---

  bool  doubleHeight = this->isSpriteDoubleHeight();
  uchar spriteHeight = (doubleHeight ? 16 : 8);

  int y1 = spriteData.y + 1; // top
  int y2 = y1 + spriteHeight;

  if (y < y1 || y > y2)
    return false;

  //---

  int x = spriteData.x;

  int iby = y - y1;

  int iby1 = (spriteData.flipY ? spriteHeight - 1 - iby : iby);
  int iby1 = (spriteData.flipY ? spriteHeight - 1 - iby : iby);

  ushort p;

  if (! doubleHeight) {
    p = spritePatternAddr() + spriteData.pattern*16 + iby1;
  }
  else {
    if (iby < 8)
      p = spritePatternAddr() + spriteData.pattern*16 + iby1;
    else
      p = spritePatternAltAddr() + spriteData.pattern*16 + iby1 - 8;
  }

  uchar c1 = getVRAMByte(p    ); // color bit 0
  uchar c2 = getVRAMByte(p + 8); // color bit 1

  // TODO: flip X ?
  for (int ibx = 0; ibx < 8; ++ibx) {
    if (spriteMask_ && x + ibx < 8)
      continue;

    // get color bits 0 and 1 from sprite pattern
    bool b1 = (c1 & (1 << (7 - ibx)));
    bool b2 = (c2 & (1 << (7 - ibx)));

    if (b1 || b2)
      return true;
  }

  return false;
}
#endif

}
