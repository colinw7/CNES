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

  mem_ = new uchar [0x4000];
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
  // With vertical mirroring, tables 2 and 3 are the mirrors of pages 0 and 1 appropriately.
  // With horizontal mirroring, pages 1 and 3 are the mirrors of pages 0 and 2 appropriately.

  // Pattern Table 0 (256x2x8, may be VROM)
  if      (addr < 0x1000) {
  }
  // Pattern Table 1 (256x2x8, may be VROM)
  else if (addr < 0x2000) {
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
    // TODO: The $3F00 and $3F10 locations in VRAM mirror each other (i.e. it
    // is the same memory cell) and define the background color of the picture.
  }
  // Sprite Palette
  else if (addr < 0x3F20) {
  }
  // Empty
  else if (addr < 0x4000) {
  }

  return mem_[addr & 0x3FFF];
}

void
PPU::
setByte(ushort addr, uchar c)
{
  mem_[addr & 0x3FFF] = c;
}

void
PPU::
drawLines()
{
  for (int y = 0; y < s_numLines; ++y)
    drawLine(y);
}

void
PPU::
drawNameTable()
{
  auto *cpu = machine_->getCPU();

  ushort nameTableAddr = cpu->nameTableAddr(); // TODO: mirroring

  //---

  uchar bg = getByte(0x3F00);

  fillScreen(bg);

  //---

  // TODO: include Attribute Table colors

  // name table
  for (int iy = 0; iy < 32; ++iy) {
    int y = iy*8;

    int iay = iy/2;

    for (int ix = 0; ix < 32; ++ix, ++nameTableAddr) {
      int x = ix*8;

      int iax = ix/2;

      uchar c = getByte(nameTableAddr); // tile number

      uchar ac = getByte(0x3F00 + iay*16 + iax);

      uchar ac1 = 0;

      if (! (y & 1)) {
        if (! (x & 1)) ac1 = ac & 0x03;
        else           ac1 = ac & 0x0C;
      }
      else {
        if (! (x & 1)) ac1 = ac & 0x30;
        else           ac1 = ac & 0xC0;
      }

      drawChar(x, y, c, ac1);
    }
  }
}

void
PPU::
drawLine(int y)
{
  lineNum_ = y;

  // vertical sync
  if      (lineNum_ < s_vsyncLines) {
  }
  // vblank 1
  else if (lineNum_ < s_vsyncLines + s_vblank1Lines) {
    if (lineNum_ == s_vsyncLines) {
      // set vblank ?
    }
  }
  // screen
  else if (lineNum_ < s_vsyncLines + s_vblank1Lines + s_visibleLines) {
    vblank_ = false;

    int y1 = lineNum_ - s_vsyncLines - s_vblank1Lines;
    int iy = y1/32;

    int x = 0;

    // hblank 1
    for (int x1 = 0; x1 < s_leftMargin; ++x1)
      ++x;

    auto *cpu = machine_->getCPU();

    ushort nameTableAddr = cpu->nameTableAddr() + iy*32;

    for (int ix = 0; ix < 32; ++ix, x += 8, ++nameTableAddr) {
      uchar c = getByte(nameTableAddr); // tile number

      uchar ac1 = 0;

#if 0
      int iax = ix/2;
      int iay = iy/2;

      uchar ac = getByte(0x3F00 + iay*16 + iax);

      if (! (y1 & 1)) {
        if (! (x & 1)) ac1 = ac & 0x03;
        else           ac1 = ac & 0x0C;
      }
      else {
        if (! (x & 1)) ac1 = ac & 0x30;
        else           ac1 = ac & 0xC0;
      }
#endif

      drawCharLine(x, y1, c, ac1);
    }

    // hblank 2
    for (int x1 = 0; x1 < s_rightMargin; ++x1)
      ++x;
  }
  // vblank 2
  else {
    // NMI on first vblank line (if enabled)
    if (lineNum_ == 257) {
      vblank_    = true;
      spriteHit_ = false;

      auto *cpu = machine_->getCPU();

      if (cpu->isBlankInterrupt()) {
        if (! cpu->isHalt())
          cpu->resetNMI();
      }
    }
  }
}

void
PPU::
drawChar(int x, int y, uchar c, uchar /*ac*/)
{
  auto *cart = machine_->getCart();

  cart->drawPPUChar(0, x, y, c);
}

void
PPU::
drawCharLine(int x, int y, uchar c, uchar /*ac*/)
{
  int iby = (y & 0x07);
  int y1  = y - iby;

  y1 += s_vsyncLines + s_vblank1Lines;

  auto *cpu = machine_->getCPU();

  if (cpu->isScreenVisible()) {
    auto *cart = machine_->getCart();

    cart->drawPPUCharLine(0, x, y1, c, iby);
  }
  else {
    setColor(0);

    for (int ibx = 0; ibx < 8; ++ibx)
      drawPixel(x + ibx, y + iby);
  }
}

}
