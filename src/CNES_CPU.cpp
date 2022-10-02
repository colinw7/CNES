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
  if      (addr <= 0x1FFF) {
    uchar c = C6502::getByte(addr & 0x07FF);

    if (isDebugRead() && ! in_ppu_ && ! isDebugger())
      std::cerr << "CPU::getByte (Internal RAM) " <<
        std::hex << addr << " " << std::hex << int(c) << "\n";

    return c;
  }
  // Input/Output
  else if (addr >= 0x2000 && addr <= 0x4FFF) {
    uchar c = 0x00;

    // PPU Control registers
    if      (addr >= 0x2000 && addr <= 0x2007) {
      auto *ppu = machine_->getPPU();

      c = ppu->getControlByte(addr);
    }
    // Joystick 1 + Strobe
    else if (addr == 0x4016) {
      if (! isDebugger()) {
        auto *ppu = machine_->getPPU();

        c = 0x40;

        if (ppu->isKey1(keyNum1_))
          c |= 0x01;

        keyNum1_ = ((keyNum1_ + 1) & 0x07);
      }
      else
        c = C6502::getByte(addr);
    }
    // Joystick 2
    else if (addr == 0x4017) {
      // Not connected
      if (! isDebugger()) {
#if 0
        auto *ppu = machine_->getPPU();

        c = 0x40;

        if (ppu->isKey2(keyNum2_))
          c |= 0x01;

        keyNum2_ = ((keyNum2_ + 1) & 0x07);

        if (isDebugRead() && ! in_ppu_)
          std::cerr << "CPU::getPPUByte " <<
             std::hex << addr << " " << std::hex << int(c) << "\n";
#else
        c = 0x42;
#endif
      }
      else
        c = C6502::getByte(addr);
    }
    else {
      c = C6502::getByte(addr);
    }

    if (isDebugRead() && ! in_ppu_ && ! isDebugger())
      std::cerr << "CPU::getPPUByte " <<
        std::hex << addr << " " << std::hex << int(c) << "\n";

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
  else if (addr >= 0xC000) {
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
  if      (addr <= 0x1FFF) {
    if (isDebugWrite() && ! isDebugger())
      std::cerr << "CPU::setByte (Internal RAM) " <<
        std::hex << addr << " " << std::hex << int(c) << "\n";

    addr &= 0x7FF;
  }
  // Input/Output
  else if (addr >= 0x2000 && addr <= 0x4FFF) {
    if (isDebugWrite() && ! isDebugger())
      std::cerr << "CPU::setByte (Input/Output) " <<
        std::hex << addr << " " << std::hex << int(c) << "\n";

    // PPU Control registers
    if      (addr >= 0x2000 && addr <= 0x2007) {
      auto *ppu = machine_->getPPU();

      ppu->setControlByte(addr, c);
    }

    else if (addr >= 0x3F00 && addr <= 0x3FFF) {
      // $3F00 and $3F10 locations in VRAM mirror each other
    }

    // Sound
    else if (addr >= 0x4000 && addr <= 0x4013) {
      // TODO
#if 0
      if      (addr == 0x4000) sq1Vol_    = c;
      else if (addr == 0x4001) sq1Sweep_  = c;
      else if (addr == 0x4002) sq1Lo_     = c;
      else if (addr == 0x4003) sq1Hi_     = c;
      else if (addr == 0x4004) sq2Vol_    = c;
      else if (addr == 0x4005) sq2Sweep_  = c;
      else if (addr == 0x4006) sq2Lo_     = c;
      else if (addr == 0x4007) sq2Hi_     = c;
      else if (addr == 0x4008) triLinear_ = c;
//    else if (addr == 0x4009) { } // unused
      else if (addr == 0x400a) triLo_     = c;
      else if (addr == 0x400b) triHi_     = c;
      else if (addr == 0x400c) noiseVol_  = c;
//    else if (addr == 0x400d) { } // unused
      else if (addr == 0x400e) noiseLo_   = c;
      else if (addr == 0x400f) noiseHi_   = c;
      else if (addr == 0x4010) dmcFreq_   = c;
      else if (addr == 0x4011) dmcRaw_    = c;
      else if (addr == 0x4012) dmcStart_  = c;
      else if (addr == 0x4013) dmcLen_    = c;
#endif
      if (! isDebugger())
        std::cerr << "CPU::setByte (Sound) " <<
          std::hex << addr << " " << std::hex << int(c) << "\n";
    }
    // DMA Access to the Sprite Memory (OAMDMA)
    else if (addr == 0x4014) {
      auto *ppu = machine_->getPPU();

      ppu->copySpriteMem(c);

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
    else if (addr >= 0x4018 && addr <= 0x401F) {
      // APU and I/O functionality that is normally disabled.
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
  else if (addr >= 0xC000) {
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
