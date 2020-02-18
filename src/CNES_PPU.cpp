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

  mem_ = new uchar [0x4000]; // 16k

  int np = s_visibleLines*s_visiblePixels;

  pixels_.resize(np);

  memset(&pixels_[0], 0, np*sizeof(ushort));
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
  }
  // Attribute Table 2
  else if (addr < 0x2C00) {
  }
  // Name Table 3 (32x25 tiles)
  else if (addr < 0x2FC0) {
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

void
PPU::
setByte(ushort addr, uchar c)
{
  auto *cpu = machine_->getCPU();

  if (isDebugWrite() && ! cpu->isDebugger())
    std::cerr << "PPU::setByte " <<
          std::hex << addr << " " << std::hex << int(c) << "\n";

  // The $3F00 and $3F10 locations in VRAM mirror each other (i.e. it
  // is the same memory cell) and define the background color of the picture.
  if (addr == 0x3F10)
    addr = 0x3F00;

  mem_[addr & 0x3FFF] = c;

  memChanged(addr, 1);
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
  auto *cpu = machine_->getCPU();

  ushort nameTableAddr = cpu->nameTableAddr(); // TODO: mirroring

  //---

  uchar bg = palette(0);

  fillScreen(bg);

  //---

  // TODO: include Attribute Table colors

  // name table
  for (int iy = 0; iy < 32; ++iy) {
    int y = iy*8;

    for (int ix = 0; ix < 32; ++ix, ++nameTableAddr) {
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

  lineNum_ = y;

  if (! spriteHit_) {
    if (isSprite0Hit(lineNum_)) {
      spriteHit_ = true;

      auto *cpu = machine_->getCPU();

      if (cpu->isSpriteInterrupt()) {
        if (! cpu->isHalt() && ! cpu->inNMI())
          cpu->resetNMI();
      }
    }
  }

  // vertical sync
  if      (lineNum_ < s_vsyncLines) {
  }
  // vblank 1
  else if (lineNum_ < s_topMargin) {
  }
  // screen
  else if (lineNum_ < s_topMargin + s_visibleLines) {
    // NOTE: sprite evaluation starts at line 65 (3 + 14 + 48 ?)
    auto *cpu = machine_->getCPU();

    vblank_ = false;

    //---

    int y1 = lineNum_ - s_topMargin; // y pixel position (0 - 240)

    int sy  = scrollV_;   // scroll vertical
    int syb = sy/8;       // scroll vertical bytes
    int syo = sy - syb*8; // scroll vertical byte pixel offset

    int iy  = y1/8;      // vertical byte index in associated char data
    int iby = y1 - iy*8; // sub pixel in vertical byte

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

    if (cpu->isScreenVisible()) {
      int sx   = cpu->scrollH(); // scroll horizontal
      int sxb  = sx/8;           // scroll horizontal bytes
      int sxo  = sx - sxb*8;     // scroll horizontal byte pixel offset
      int isxo = 8 - sxo;        // inverse of scroll horizontal byte pixel offset

      // draw partial left character
      if (sxo > 0) {
        // get tile number and attribute color
        uchar c  = calcNameTableTile (iy1, -sxb - 1);
        uchar ac = calcAttrTableColor(iy1, -sxb - 1);

        //---

        drawCharLine(x1 - isxo, y1, c, ac, iby1, isxo, 8);
      }

      for (int ix = 0; ix < 32; ++ix, x1 += 8) {
        int ix1 = ix - sxb; // horizontal byte index in associated char data

        //---

        // get tile number and attribute color
        uchar c  = calcNameTableTile (iy1, ix1);
        uchar ac = calcAttrTableColor(iy1, ix1);

        //---

        if (ix == 31 && sxo > 0)
          drawCharLine(x1 + sxo, y1, c, ac, iby1, 0, isxo);
        else {
          drawCharLine(x1 + sxo, y1, c, ac, iby1, 0, 8);
        }
      }
    }
    else {
      // blank line
      uchar color = palette(0);

      for (int ix = 0; ix < 256; ++ix, ++x1)
        drawColorPixel(ix, y1, color);
    }

    //---

    drawSpritesOnLine(y1);

    //---

    // hblank 2
    x = x1 + s_leftMargin;

    for (int ix = 0; ix < s_rightMargin; ++ix)
      ++x;
  }
  // vblank 2
  else {
    auto *cpu = machine_->getCPU();

    cpu->setSpriteAddr(0); // reset

    // NMI on first vblank line (if enabled)
    if (lineNum_ == s_vblankLine) {
      //cpu->resetScroll();

      //---

      vblank_    = true;
      spriteHit_ = false;

      if (cpu->isBlankInterrupt()) {
        if (! cpu->isHalt() && ! cpu->inNMI())
          cpu->resetNMI();
      }
    }

    if (lineNum_ == s_numLines - 1) {
      scrollV_ = cpu->scrollV();
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
  auto *cpu = machine_->getCPU();

  int y1 = y - iby; // adjust to top of char

  ushort p = cpu->screenPatternAddr() + c*16 + iby;

  uchar c1 = getVRAMByte(p    ); // color bit 0
  uchar c2 = getVRAMByte(p + 8); // color bit 1

  for (int ibx = ix; ibx < nx; ++ibx) {
    bool b1 = (c1 & (1 << (7 - ibx)));
    bool b2 = (c2 & (1 << (7 - ibx)));

    uchar color = palette((b1 | (b2 << 1)) | ac);

    drawColorPixel(x + ibx, y1 + iby, color);
  }
}

uchar
PPU::
calcNameTableTile(int iy, int ix) const
{
  if (iy < 0) iy += 30;
  if (ix < 0) ix += 32;

  assert(ix >= 0 && iy >= 0);

  auto *cpu = machine_->getCPU();

  // 32 bytes per line in name table (8 pixels high)
  ushort offset = 32*iy + ix;

  assert(offset < 0x03C0);

  ushort nameTableAddr = cpu->nameTableAddr() + offset;

  return getByte(nameTableAddr); // tile number
}

uchar
PPU::
calcAttrTableColor(int iy, int ix) const
{
  if (iy < 0) iy += 30;
  if (ix < 0) ix += 32;

  assert(ix >= 0 && iy >= 0);

  int iay = iy >> 2;
  int iax = ix >> 2;

  auto *cpu = machine_->getCPU();

  ushort offset = 0x03C0 + iay*8 + iax;

  assert(offset < 0x0400);

  ushort attrTableAddr = cpu->nameTableAddr() + offset;

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
  auto *cpu = machine_->getCPU();

  bool visible = cpu->isSpritesVisible();
  if (! visible) return;

  auto *cart = machine_->getCart();

  //---

  uchar spriteAddr   = cpu->spriteAddr();
  bool  doubleHeight = cpu->isSpriteDoubleHeight();
//bool  masked       = cpu->isSpriteMasked();

  for (int i = 0; i < 64; ++i) {
    int ispriteAddr = spriteAddr + 4*i;

    if (ispriteAddr > 252) // out of bounds
      break;

    int pattern = cpu->spriteMem(ispriteAddr + 1);

    int x = cpu->spriteMem(ispriteAddr + 3);
    int y = cpu->spriteMem(ispriteAddr + 0) + 1; // next line

    uchar m2 = cpu->spriteMem(ispriteAddr + 2);

    uchar c1     = (m2 & 0x03) << 2;
//  bool  behind = m2 & 0x20;
//  bool  flipX  = m2 & 0x40;
//  bool  flipY  = m2 & 0x80;

    cart->drawSpriteChar(x, y, pattern, c1, doubleHeight);
  }
}
#endif

void
PPU::
drawSpritesOnLine(int y)
{
  auto *cpu = machine_->getCPU();

  bool visible = cpu->isSpritesVisible();
  if (! visible) return;

  //---

  uchar spriteAddr   = cpu->spriteAddr();
  bool  doubleHeight = cpu->isSpriteDoubleHeight();

  //---

  using VisibleSprites = std::vector<int>;

  VisibleSprites visibleSprites;

  for (int i = 0; i < 64; ++i) {
    int ispriteAddr = spriteAddr + 4*i;

    if (ispriteAddr > 252) // out of bounds
      break;

    int y1 = cpu->spriteMem(ispriteAddr + 0) + 1; // top
    int y2 = (y1 + (doubleHeight ? 16 : 8));

    if (y2 < y || y1 > y)
      continue;

    visibleSprites.push_back(i);
  }

  //----

  int ns = std::min(visibleSprites.size(), 8UL);

  for (int i = 0; i < ns; ++i)
    drawSpriteLine(visibleSprites[i], y);
}

void
PPU::
drawSpriteLine(int i, int y)
{
  in_ppu_ = true;

  auto *cpu = machine_->getCPU();

  //---

  uchar spriteAddr = cpu->spriteAddr();
//bool  masked     = cpu->isSpriteMasked();

  //---

  int ispriteAddr = spriteAddr + 4*i;

  //---

  int pattern = cpu->spriteMem(ispriteAddr + 1);

  int x  = cpu->spriteMem(ispriteAddr + 3);
  int y1 = cpu->spriteMem(ispriteAddr + 0) + 1; // top

  uchar m2 = cpu->spriteMem(ispriteAddr + 2);

  uchar c1     = (m2 & 0x03) << 2;
//bool  behind = m2 & 0x20;
//bool  flipX  = m2 & 0x40;
//bool  flipY  = m2 & 0x80;

  drawSpriteCharLine(x, y, pattern, y - y1, c1);

  in_ppu_ = false;
}

// draw nth sprite line (iby) for sprite at (x, y) and character (c) and
// bits 2 and 3 (ac)
void
PPU::
drawSpriteCharLine(int x, int y, uchar c, int iby, uchar ac)
{
  auto *cpu = machine_->getCPU();

  ushort p;

  if (iby < 8)
    p = cpu->spritePatternAddr() + c*16 + iby;
  else
    p = cpu->spritePatternAltAddr() + c*16 + iby - 8;

  uchar c1 = getVRAMByte(p    ); // color bit 0
  uchar c2 = getVRAMByte(p + 8); // color bit 1

  for (int ibx = 0; ibx < 8; ++ibx, ++x) {
    if (x < 0 || x >= s_visiblePixels)
      continue;

    // get color bits 0 and 1 from sprite pattern
    bool b1 = (c1 & (1 << (7 - ibx)));
    bool b2 = (c2 & (1 << (7 - ibx)));

    uchar color = spritePalette((b1 | (b2 << 1)) | ac);

    drawColorPixel(x, y, color);
  }
}

void
PPU::
drawColorPixel(int x, int y, uchar color)
{
  assert(x >= 0 && x < s_visiblePixels);
  assert(y >= 0 && y < s_visibleLines);

  int ind = y*s_visiblePixels + x;

  auto *cpu = machine_->getCPU();

  if (cpu->isGrayScale())
    color &= 0x30;

  uchar ec { 0 };

  if (cpu->isEmphasizeRed  ()) ec |= 0x01;
  if (cpu->isEmphasizeGreen()) ec |= 0x02;
  if (cpu->isEmphasizeBlue ()) ec |= 0x04;

  ushort pixel = (ec << 8) | color;

  if (pixels_[ind] != pixel) {
    pixels_[ind] = pixel;

    //---

    setColor(color);

    drawPixel(x + s_leftMargin, y + s_topMargin);
  }
}

bool
PPU::
isSprite0Hit(int y) const
{
  // Sprite 0 hit does not trigger in any area where the background or sprites are hidden

  auto *cpu = machine_->getCPU();

  bool visible = cpu->isSpritesVisible();

  if (! visible)
    return false;

  uchar spriteAddr   = cpu->spriteAddr();
  bool  doubleHeight = cpu->isSpriteDoubleHeight();

  int y1 = cpu->spriteMem(spriteAddr + 0) + 1; // top
  int y2 = (y1 + (doubleHeight ? 16 : 8));

  if (y < y1 || y > y2)
    return false;

  int pattern = cpu->spriteMem(spriteAddr + 1);

  int iby = y - y1;

  ushort p;

  if (iby < 8)
    p = cpu->spritePatternAddr() + pattern*16 + iby;
  else
    p = cpu->spritePatternAltAddr() + pattern*16 + iby - 8;

  uchar c1 = getVRAMByte(p    ); // color bit 0
  uchar c2 = getVRAMByte(p + 8); // color bit 1

  for (int ibx = 0; ibx < 8; ++ibx) {
    // get color bits 0 and 1 from sprite pattern
    bool b1 = (c1 & (1 << (7 - ibx)));
    bool b2 = (c2 & (1 << (7 - ibx)));

    if (b1 || b2)
      return true;
  }

  return false;
}

}
