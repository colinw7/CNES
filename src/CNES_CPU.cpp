#include <CNES_CPU.h>
#include <CNES_Machine.h>
#include <CNES_PPU.h>
#include <CNES_Cartridge.h>
#include <C6502.h>

namespace CNES {

CPU::
CPU(Machine *machine) :
 C6502(), machine_(machine)
{
  // enable unsupported 6502 instructons
  setUnsupported(true);
}

CPU::
~CPU()
{
}

// get byte (called by ppu)
uchar
CPU::
ppuGetByte(ushort addr) const
{
  in_ppu_ = true;

  uchar c = getByte(addr);

  in_ppu_ = false;

  return c;
}

uchar
CPU::
getByte(ushort addr) const
{
  // 2kB Internal RAM, mirrored 4 times
  if      (addr >= 0x0000 && addr <= 0x1FFF) {
    uchar c = C6502::getByte(addr & 0x07FF);

    if (isDebugRead() && ! in_ppu_ && ! isDebugger())
      std::cerr << "CPU::getByte (Internal RAM) " <<
        std::hex << addr << " " << std::hex << int(c) << "\n";

    return c;
  }
  // Input/Output
  else if (addr >= 0x2000 && addr <= 0x4FFF) {
    auto *ppu = machine_->getPPU();

    uchar c = 0x00;

    // PPU Status Register (PPUSTATUS)
    if      (addr == 0x2002) {
      // read only
      c = 0x00; // TODO: Least significant bits previously written into a PPU register

      // TODO: Sprite overflow on bit 5

      if (ppu->isSpriteHit())
        c |= 0x40;

      if (ppu->isVBlank())
        c |= 0x80;

      // reset on read
      if (! isDebugger()) {
      //ppu->setSpriteHit(false);
        ppu->setVBlank   (false);

        byteId_ = 0;
      }

      if (isDebugRead() && ! in_ppu_ && ! isDebugger())
        std::cerr << "CPU::getPPUByte " << std::hex << addr << " " << std::hex << int(c) << "\n";
    }
    // Sprite Memory Data (OAMDATA)
    else if (addr == 0x2004) {
      if (! isDebugger()) {
        c = spriteMem_[spriteAddr_++];

        if (isDebugRead() && ! in_ppu_)
          std::cerr << "CPU::getSpriteByte " <<
            std::hex << addr << " " << std::hex << int(c) << "\n";
      }
      else
        c = C6502::getByte(addr);
    }
    // PPU Memory Data (PPUDATA)
    else if (addr == 0x2007) {
      if (! isDebugger()) {
        if (ppuAddr_ < 0x3F00) {
          c = ppuBuffer_;

          ppuBuffer_ = ppu->getByte(ppuAddr_);
        }
        else {
          c = ppu->getByte(ppuAddr_);

          ppuBuffer_ = ppu->getByte(ppuAddr_); // TODO: mirrored nametable data ?
        }

        ++ppuAddr_;
      }
      else
        c = ppuBuffer_;
    }
    // Joystick 1 + Strobe
    else if (addr == 0x4016) {
      if (! isDebugger()) {
        c = 0x40;

        if (ppu->isKey1(keyNum1_))
          c |= 0x01;

        keyNum1_ = ((keyNum1_ + 1) & 0x07);

        if (isDebugRead() && ! in_ppu_)
          std::cerr << "CPU::getPPUByte " << std::hex << addr << " " << std::hex << int(c) << "\n";
      }
      else
        c = C6502::getByte(addr);
    }
    // Joystick 2
    else if (addr == 0x4017) {
      // Not connected
      if (! isDebugger()) {
#if 0
        c = 0x40;

        if (ppu->isKey2(keyNum2_))
          c |= 0x01;

        keyNum2_ = ((keyNum2_ + 1) & 0x07);

        if (isDebugRead() && ! in_ppu_)
          std::cerr << "CPU::getPPUByte " << std::hex << addr << " " << std::hex << int(c) << "\n";
#else
        c = 0x42;
#endif
      }
      else
        c = C6502::getByte(addr);
    }
    else {
      c = C6502::getByte(addr);

      if (isDebugRead() && ! in_ppu_ && ! isDebugger())
        std::cerr << "CPU::getPPUByte " << std::hex << addr << " " << std::hex << int(c) << "\n";
    }

    return c;
  }
  // Expansion Modules
  else if (addr >= 0x5000 && addr <= 0x5FFF) {
    uchar c = C6502::getByte(addr);

    if (isDebugRead() && ! in_ppu_ && ! isDebugger())
      std::cerr << "CPU::getByte (Expansion Module) " <<
        std::hex << addr << " " << std::hex << int(c) << "\n";

    return c;
  }
  // Cartridge RAM (may be battery-backed)
  else if (addr >= 0x6000 && addr <= 0x7FFF) {
    uchar c = C6502::getByte(addr);

    if (isDebugRead() && ! in_ppu_ && ! isDebugger())
      std::cerr << "CPU::getByte (Cartridge RAM) " <<
        std::hex << addr << " " << std::hex << int(c) << "\n";

    return c;
  }
  // Lower Bank of Cartridge ROM (16k)
  else if (addr >= 0x8000 && addr <= 0xBFFF) {
    auto *cart = machine_->getCart();

    uchar c;

    // cartridge code
    if (! cart->getLowerROMByte(addr - 0x8000, c))
      c = C6502::getByte(addr);

    return c;
  }
  // Upper Bank of Cartridge ROM (16k)
  else if (addr >= 0xC000 && addr <= 0xFFFF) {
    auto *cart = machine_->getCart();

    uchar c;

    // cartridge code
    if (! cart->getUpperROMByte(addr - 0xC000, c))
      c = C6502::getByte(addr);

    return c;
  }
  else {
    assert(false);
    return 0;
  }
}

void
CPU::
setByte(ushort addr, uchar c)
{
  // 2kB Internal RAM, mirrored 4 times
  if      (addr >= 0x0000 && addr <= 0x1FFF) {
    if (isDebugWrite() && ! isDebugger())
      std::cerr << "CPU::setByte (Internal RAM) " <<
        std::hex << addr << " " << std::hex << int(c) << "\n";

    addr &= 0x7FF;
  }
  // Input/Output
  else if (addr >= 0x2000 && addr <= 0x4FFF) {
    auto *ppu = machine_->getPPU();

    if (isDebugWrite() && ! isDebugger())
      std::cerr << "CPU::setByte (Input/Output) " <<
        std::hex << addr << " " << std::hex << int(c) << "\n";

    // PPU Control Register 1 (PPUCTRL)
    if      (addr == 0x2000) {
      nameTable_          = (c & 0x3); // TODO: mirroring ?
      nameTableAddr_      = 0x2000 + nameTable_*0x400; // TODO: mirroring ?
      verticalWrite_      = (c & 0x04);
      spritePatternAddr_  = (c & 0x08 ? 0x1000 : 0x0000);
      screenPatternAddr_  = (c & 0x10 ? 0x1000 : 0x0000);
      spriteDoubleHeight_ = (c & 0x20);
//    spriteInterrupt_    = (c & 0x40); // TODO: wrong
      blankInterrupt_     = (c & 0x80);

      spritePatternAltAddr_ = (spritePatternAddr_ == 0x1000 ? 0x0000 : 0x100);
    }
    // PPU Control Register 2 (PPUMASK)
    else if (addr == 0x2001) {
      grayScale_      = (c & 0x01);
      imageMask_      = (c & 0x02); // TODO
      spriteMask_     = (c & 0x04); // TODO
      screenVisible_  = (c & 0x08);
      spritesVisible_ = (c & 0x10);
      emphasizeRed_   = (c & 0x20);
      emphasizeGreen_ = (c & 0x40);
      emphasizeBlue_  = (c & 0x80);
    }
    // PPU Status Register (PPUSTATUS)
    else if (addr == 0x2002) {
      // read only
    }
    // Sprite Memory Address (OAMADDR)
    else if (addr == 0x2003) {
      setSpriteAddr(c);
    }
    // Sprite Memory Data (OAMDATA)
    else if (addr == 0x2004) {
      spriteMem_[spriteAddr_++] = c;
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
      ppuAddrHL_[byteId_] = c;

      byteId_ = 1 - byteId_;

      ppuAddr_ = (ppuAddrHL_[0] << 8) | ppuAddrHL_[1];
      ppuAddr_ &= 0x3FFF;
    }
    // PPU Memory Data (PPUDATA)
    else if (addr == 0x2007) {
      ppu->setByte(ppuAddr_, c);

      if (verticalWrite_)
        ppuAddr_ += 32;
      else
        ++ppuAddr_;
    }

    //---

    else if (addr >= 0x3F00 && addr <= 0x3FFF) {
      // $3F00 and $3F10 locations in VRAM mirror each other
    }

    //---

    // Sound
    if      (addr >= 0x4000 && addr <= 0x4013) {
      // TODO
    }
    // DMA Access to the Sprite Memory (OAMDMA)
    else if (addr == 0x4014) {
      C6502::memget(c << 8, &spriteMem_[0], 0x100);

      tick(255); tick(255); tick(3); // 513 or 514 ?
    }
    // Sound Switch
    else if (addr == 0x4015) {
      // TODO
      //soundEnabled_[0] = (c & 0x01);
      //soundEnabled_[1] = (c & 0x02);
      //soundEnabled_[2] = (c & 0x04);
      //soundEnabled_[3] = (c & 0x08);
      //soundEnabled_[4] = (c & 0x10);
    }
    // Joystick 1 + Strobe
    else if (addr == 0x4016) {
      // TODO
      if (c == 0)
        keyNum1_ = 0;
    }
    // Joystick 2
    else if (addr == 0x4017) {
      // TODO
      if (c == 0)
        keyNum2_ = 0;
    }

    signalNesChanged();
  }
  // Expansion Modules
  else if (addr >= 0x5000 && addr <= 0x5FFF) {
    if (isDebugWrite() && ! isDebugger())
      std::cerr << "CPU::setByte (Expansion Modules) " <<
        std::hex << addr << " " << std::hex << int(c) << "\n";
  }
  // Cartridge RAM (may be battery-backed)
  else if (addr >= 0x6000 && addr <= 0x7FFF) {
    if (isDebugWrite() && ! isDebugger())
      std::cerr << "CPU::setByte (Cartridge RAM) " <<
        std::hex << addr << " " << std::hex << int(c) << "\n";
  }
  // Lower Bank of Cartridge ROM
  else if (addr >= 0x8000 && addr <= 0xBFFF) {
    auto *cart = machine_->getCart();

    if (cart->setROMByte(addr - 0x8000, c))
      return;
  }
  // Upper Bank of Cartridge ROM
  else if (addr >= 0xC000 && addr <= 0xFFFF) {
    auto *cart = machine_->getCart();

    if (cart->setROMByte(addr - 0xC000, c))
      return;
  }

  C6502::setByte(addr, c);
}

void
CPU::
memset(ushort addr, const uchar *data, ushort len)
{
  C6502::memset(addr, data, len);
}

void
CPU::
tick(uchar n)
{
  auto *ppu = machine_->getPPU();

  ppu->tick(n);
}

bool
CPU::
isScreen(ushort /*addr*/, ushort /*len*/) const
{
  return false;
}

}
