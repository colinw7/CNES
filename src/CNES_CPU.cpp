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
}

CPU::
~CPU()
{
}

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
    addr &= 0x7FF;
  }
  // Input/Output
  else if (addr >= 0x2000 && addr <= 0x4FFF) {
    auto *ppu = machine_->getPPU();

    // PPU Status Register
    if      (addr == 0x2002) {
      // read only
      uchar c = 0x00;

      if (ppu->isSpriteHit())
        c |= 0x40; // TODO

      if (ppu->isVBlank())
        c |= 0x80;

      // reset on read
      if (! isDebugger()) {
        ppu->setSpriteHit(false);
        ppu->setVBlank   (false);
      }

      if (ppu->isDebug() && ! in_ppu_ && ! isDebugger())
        std::cerr << "CPU::getByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

      return c;
    }
    // Sprite Memory Data
    else if (addr == 0x2004) {
      uchar c = spriteMem_[spriteAddr_++];

      if (ppu->isDebug() && ! in_ppu_ && ! isDebugger())
        std::cerr << "CPU::getByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

      return c;
    }
    // PPU Memory Data
    else if (addr == 0x2007) {
      uchar c = ppu->getByte(ppuAddr_++);

      if (ppu->isDebug() && ! in_ppu_ && ! isDebugger())
        std::cerr << "CPU::getByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

      return c;
    }
    // Joystick 1 + Strobe
    else if (addr == 0x4016) {
      uchar c = 0x40;

      if (ppu->isKey1(keyNum1_))
        c |= 0x01;

      keyNum1_ = ((keyNum1_ + 1) & 0x07);

      if (ppu->isDebug() && ! in_ppu_ && ! isDebugger())
        std::cerr << "CPU::getByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

      return c;
    }
    // Joystick 2
    else if (addr == 0x4017) {
      uchar c = 0x40; // TODO

      if (ppu->isKey2(keyNum2_))
        c |= 0x01;

      keyNum2_ = ((keyNum2_ + 1) & 0x07);

      if (ppu->isDebug() && ! in_ppu_ && ! isDebugger())
        std::cerr << "CPU::getByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

      return c;
    }
    else {
      uchar c = C6502::getByte(addr);

      if (ppu->isDebug() && ! in_ppu_ && ! isDebugger())
        std::cerr << "CPU::getByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

      return c;
    }
  }
  // APU, I/O Registers
  else if (addr >= 0x6000 && addr <= 0x6FFF) {
  }
  // Expansion Modules
  else if (addr >= 0x5000 && addr <= 0x5FFF) {
  }
  // Cartridge RAM (may be battery-backed)
  else if (addr >= 0x6000 && addr <= 0x7FFF) {
  }
  // Lower Bank of Cartridge ROM
  else if (addr >= 0x8000 && addr <= 0xBFFF) {
    uchar c;

    if (machine_->getCart()->getROMByte(addr - 0x8000, c)) {
      if (isDebug() && ! in_ppu_ && ! isDebugger())
        std::cerr << "CPU::getByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

      return c;
    }
  }
  // Upper Bank of Cartridge ROM
  else if (addr >= 0xC000 && addr <= 0xFFFF) {
    uchar c;

    if (machine_->getCart()->getROMByte(addr - 0x8000, c)) {
      if (isDebug() && ! in_ppu_ && ! isDebugger())
        std::cerr << "CPU::getByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

      return c;
    }
  }

  //---

  uchar c = C6502::getByte(addr);

  if (isDebug() && ! in_ppu_ && ! isDebugger())
    std::cerr << "CPU::getByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

  return c;
}

void
CPU::
setByte(ushort addr, uchar c)
{
  // 2kB Internal RAM, mirrored 4 times
  if      (addr >= 0x0000 && addr <= 0x1FFF) {
    if (isDebug() && ! in_ppu_ && ! isDebugger())
      std::cerr << "CPU::setByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

    addr &= 0x7FF;
  }
  // Input/Output
  else if (addr >= 0x2000 && addr <= 0x4FFF) {
    auto *ppu = machine_->getPPU();

    if (ppu->isDebug() && ! in_ppu_ && ! isDebugger())
      std::cerr << "CPU::setByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

    // PPU Control Register 1
    if      (addr == 0x2000) {
      nameTableAddr_      = 0x2000 + (c & 0x3)*0x400; // TODO: mirroring ?
      verticalWrite_      = (c & 0x04);
      spritePatternAddr_  = (c & 0x08 ? 0x1000 : 0x000); // TODO
      screenPatternAddr_  = (c & 0x10 ? 0x1000 : 0x000);
      spriteDoubleHeight_ = (c & 0x20); // TODO
      hitInterrupt_       = (c & 0x40); // TODO
      blankInterrupt_     = (c & 0x80);
    }
    // PPU Control Register 2
    else if (addr == 0x2001) {
      imageMask_     = (c & 0x02); // TODO
      spriteMask_    = (c & 0x04); // TODO
      screenVisible_ = (c & 0x08);
      spritesSwitch_ = (c & 0x10); // TODO
    }
    // PPU Status Register
    else if (addr == 0x2002) {
      // read only
    }
    // Sprite Memory Address
    else if (addr == 0x2003) {
      spriteAddr_ = c;
    }
    // Sprite Memory Data
    else if (addr == 0x2004) {
      spriteMem_[spriteAddr_++] = c;
    }
    // Background Scroll
    else if (addr == 0x2005) {
      if (c < 0xF0) {
        scrollXY_[scrollId_] = c; // TODO

        scrollId_ = 1 - scrollId_;
      }
    }
    // PPU Memory Address
    else if (addr == 0x2006) {
      ppuAddrHL_[ppuId_] = c;

      ppuId_ = 1 - ppuId_;

      ppuAddr_ = (ppuAddrHL_[0] << 8) | ppuAddrHL_[1];
    }
    // PPU Memory Data
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
    // DMA Access to the Sprite Memory
    else if (addr == 0x4014) {
      C6502::memget(0x100*c, &spriteMem_[0], 0x100);
    }
    // Sound Switch
    else if (addr == 0x4015) {
      // TODO
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
  }
  // Expansion Modules
  else if (addr >= 0x5000 && addr <= 0x5FFF) {
    if (isDebug() && ! in_ppu_ && ! isDebugger())
      std::cerr << "CPU::setByte " << std::hex << addr << " " << std::hex << int(c) << "\n";
  }
  // Cartridge RAM (may be battery-backed)
  else if (addr >= 0x6000 && addr <= 0x7FFF) {
    if (isDebug() && ! in_ppu_ && ! isDebugger())
      std::cerr << "CPU::setByte " << std::hex << addr << " " << std::hex << int(c) << "\n";
  }
  // Lower Bank of Cartridge ROM
  else if (addr >= 0x8000 && addr <= 0xBFFF) {
    if (isDebug() && ! in_ppu_ && ! isDebugger())
      std::cerr << "CPU::setByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

    if (machine_->getCart()->setROMByte(addr - 0x8000, c))
      return;
  }
  // Upper Bank of Cartridge ROM
  else if (addr >= 0xC000 && addr <= 0xFFFF) {
    if (isDebug() && ! in_ppu_ && ! isDebugger())
      std::cerr << "CPU::setByte " << std::hex << addr << " " << std::hex << int(c) << "\n";

    if (machine_->getCart()->setROMByte(addr - 0x8000, c))
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
tick(uchar /*n*/)
{
}

bool
CPU::
isScreen(ushort /*addr*/, ushort /*len*/) const
{
  return false;
}

}
