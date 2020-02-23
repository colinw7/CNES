#include <CNES_Cartridge.h>
#include <CNES_Machine.h>
#include <CNES_CPU.h>
#include <CNES_PPU.h>
#include <cstring>
#include <cassert>

namespace CNES {

Cartridge::
Cartridge(Machine *machine) :
 machine_(machine)
{
  for (int i = 0; i < 4; ++i)
    mapper1Data_.regData[i] = 0;

  assert(machine_);
}

bool
Cartridge::
load(const std::string &filename)
{
  auto toLower = [](const std::string &str) {
    std::string str1;

    for (const auto &c : str)
      str1 += tolower(c);

    return str1;
  };

  auto p = filename.rfind('.');
  if (p == std::string::npos) return false;

  std::string suffix = toLower(filename.substr(p));

  if (suffix != "nes")
    return loadNES(filename);
  else
    return false;
}

bool
Cartridge::
loadNES(const std::string &filename)
{
  struct File {
    File(const std::string &filename) {
      fp = fopen(filename.c_str(), "r");
    }

   ~File() {
      if (fp)
        fclose(fp);
    }

    FILE *fp { nullptr };
  };

  File file(filename);
  if (! file.fp) return false;

  //---

  auto initData = [&](std::vector<uchar> &data, ushort n) {
    data.resize(n);

    std::memset(&data[0], 0, n*sizeof(uchar));
  };

  auto readData = [&](std::vector<uchar> &data, ushort &n) {
    static uchar buffer[65526];

    if (n == 0) return true;

    ushort n1 = fread(buffer, 1, n, file.fp);
    if (n1 != n) { n = 0; return false; }

    data.resize(n);

    std::memcpy(&data[0], buffer, n*sizeof(uchar));

    return true;
  };

  //---

  Data   header;
  ushort headerSize = 16;

  if (! readData(header, headerSize))
    return false;

  // String "NES^Z" used to recognize .NES files
  if (header[0] != 0x4e || header[1] != 0x45 || header[2] != 0x53 || header[3] != 0x1a)
    return false;

  romCount_ = header[4];       // Number of 16kB ROM banks
  prgSize_  = romCount_*16384; // ROM size

  chrCount_ = header[5];      // Number of 8kB VROM banks
  chrSize_  = chrCount_*8192; // Number of 8kB VROM banks

  mirroring_    = header[6] & 0x01; // 1 for vertical mirroring, 0 for horizontal mirroring.
  batteryRAM_   = header[6] & 0x02; // 1 for battery-backed RAM at $6000-$7FFF
  trainer_      = header[6] & 0x04; // 1 for a 512-byte trainer at $7000-$71FF
  ignoreMirror_ = header[6] & 0x08; // 1 for a four-screen VRAM layout

  uchar mapperLo = (header[6] & 0xF0) >> 4; // Four lower bits of ROM Mapper Type

  // Console Type
  // 0: Nintendo Entertainment System/Family Computer
  // 1: Nintendo Vs. System
  // 2: Nintendo Playchoice 10
  // 3: Extended Console Type
  consoleType_ = header[7] & 0x03;

  nesVer_ = (header[7] & 0x0C) >> 2;

  uchar mapperHi = (header[7] & 0xF0) >> 4; // Four higher bits of ROM Mapper Type.

  if (nesVer_ == 2) {
    ver2Data.mapperHi1 = header[8] & 0x0F;
    ver2Data.subMapper = header[8] & 0xF0;

    ver2Data.prgSize1 = header[9] & 0x0F;
    ver2Data.chrSize1 = header[9] & 0xF0;

    ver2Data.prgShift1 = header[10] & 0x0F;
    ver2Data.prgShift2 = header[10] & 0xF0;

    ver2Data.chrRam1 = header[11] & 0xF0;
    ver2Data.chrRam2 = header[11] & 0xF0;

    ver2Data.ppuTiming = header[12] & 0x03;

    if      (consoleType_ == 1) {
      ver2Data.ppuType1  = header[13] & 0x0F;
      ver2Data.hardType1 = header[13] & 0xF0;
    }
    else if (consoleType_ == 3) {
      ver2Data.extType = header[13] & 0x0F;
    }

    ver2Data.miscRoms = header[14] & 0x03;

    ver2Data.defExp = header[15] & 0x3F;
  }
  else {
    prgCount_   = (header[8] ? header[8] : 1); // Number of 8kB RAM banks
    prgRamSize_ = prgCount_*8192;              // RAM size

    tvType_       = header[9] & 0x03;     // 1 for PAL cartridges, otherwise assume NTSC
    hasPrgRam_    = ! (header[9] & 0x10);
    busConflicts_ = header[9] & 0x20;
  }

  mapper_ = mapperLo | (mapperHi << 4);

  //---

  if (trainer_) {
    ushort trainerSize = 512;

    if (! readData(trainerData_, trainerSize))
      return false;
  }

  //---

  // Cartridge ROM (mapped to $8000-$FFFF)
  if (! readData(prgRomData_, prgSize_))
    return false;

  //---

  // character rom
  if (! readData(chrRomData_, chrSize_))
    return false;

  //---

  // character ram (optional)
  if (hasPrgRam_) {
    if (! readData(prgRamData_, prgRamSize_))
      initData(prgRamData_, prgRamSize_);
  }

  //---

  if (consoleType_ == 2) {
    ushort playChoiceSize = 8192;

    if (! readData(playChoiceData_, playChoiceSize))
      return false;

    ushort playExtraSize = 32;

    if (! readData(playExtraData_, playExtraSize))
      return false;
  }

  updateState();

  return true;
}

// Cartridge Lower ROM (mapped to $8000-$BFFF)
bool
Cartridge::
getLowerROMByte(ushort addr, uchar &c) const
{
  if (romCount_ < 2)
    return false;

  assert(addr < prgSize_);

  c = prgRomData_[addr];

  auto cpu = machine_->getCPU();

  if (isDebugRead() && ! cpu->isDebugger())
    std::cerr << "Cartridge::getLowerROMByte " <<
      std::hex << addr << " " << std::hex << int(c) << "\n";

  return true;
}

// Cartridge Upper ROM (mapped to $C000-$FFFF)
bool
Cartridge::
getUpperROMByte(ushort addr, uchar &c) const
{
  if (romCount_ < 1)
    return false;

  if (romCount_ >= 2)
    addr += 0x4000;

  assert(addr < prgSize_);

  c = prgRomData_[addr];

  auto cpu = machine_->getCPU();

  if (isDebugRead() && ! cpu->isDebugger())
    std::cerr << "Cartridge::getUpperROMByte " <<
      std::hex << addr << " " << std::hex << int(c) << "\n";

  return true;
}

bool
Cartridge::
setROMByte(ushort addr, uchar c)
{
  auto cpu = machine_->getCPU();

  if (isDebugWrite() && ! cpu->isDebugger())
    std::cerr << "Cartridge::setROMByte " <<
      std::hex << addr << " " << std::hex << int(c) << "\n";

  if      (mapper_ == 1) {
    if (c & 0x80) {
      mapper1Data_.regBit   = 0;
      mapper1Data_.regValue = 0;

      if (isDebugWrite())
        std::cerr << "Cartridge::setROMByte : reset mapper\n";
    }
    else {
      uchar b = c & 0x01;

      if (b)
        mapper1Data_.regValue |= (1 << mapper1Data_.regBit);

      ++mapper1Data_.regBit;

      if (mapper1Data_.regBit > 8)
        mapper1Data_.regBit = 0;

      if (mapper1Data_.regBit == 5) {
        int regNum = 0;

        if      (addr < 0x2000) regNum = 0; // $8000-$9FFF
        else if (addr < 0x4000) regNum = 1; // $A000-$BFFF
        else if (addr < 0x6000) regNum = 2; // $C000-$DFFF
        else if (addr < 0x8000) regNum = 3; // $E000-$FFFF

        mapper1Data_.regData[regNum] = mapper1Data_.regValue;

        //---

        // 0: one-screen, lower bank (nametable 0)
        // 1: one-screen, upper bank (nametable 1)
        // 2: vertical mirror
        // 3: horizontal mirror
        mapper1Data_.mirror = mapper1Data_.regData[0] & 0x03;

        // PRG ROM swap bank
        //  0 - Bank 8000-BFFFh is fixed, while C000-FFFFh is swappable
        //  1 - Bank C000-FFFFh is fixed, while 8000-FFFFh is swappable. (power-on default)
        mapper1Data_.romBank = (mapper1Data_.regData[0] & 0x04) >> 2;

        // PRG Bank size.
        //  0 - Swappable bank is 32K in size.
        //  1 - Swappable bank is 16K in size.
        mapper1Data_.romSize = (mapper1Data_.regData[0] & 0x08) >> 3;

        // CHR ROM bank mode
        //  0: Single 8K bank in CHR space
        //  1: Two 4K banks in CHR space
        mapper1Data_.vromMode = (mapper1Data_.regData[0] & 0x10) >> 4;

        // CHR bank 0
        // If in 4K bank mode, this selects a 4K bank at 0000h on the PPU space.
        // If in 8K bank mode, this selects a full 8K bank at 0000h on the PPU space.
        // Note: If using 8K banks, the lowest bit (D0) is NOT USED.
        // Note: some of these bits are commondeered for different cart boards
        // (explained on the individual mapper pages).
        mapper1Data_.vromBank[0] = mapper1Data_.regData[1] & 0x1F;

        // CHR bank 1
        // If in 4K bank mode, this selects a 4K bank at 1000h on the PPU space.
        // If in 8K bank mode, this register does nothing.
        mapper1Data_.vromBank[1] = mapper1Data_.regData[2] & 0x1F;

        // PRG bank.
        // . If in 32K mode, this selects a full 32K bank in the PRG space.
	//   Only the upper 3 bits are used then.
        // . If in 16K mode, this selects a 16K bank in either 8000-BFFFh
	//   or C000-FFFFh depending on the state of the "H" bit in register 0.
        mapper1Data_.romPage = mapper1Data_.regData[3] & 0x0F;

        // PRG RAM chip enable
        //  0- WRAM is enabled and can be read/written to.
        //  1- WRAM is disabled and cannot be accessed at all. Reading results in open bus.
        //     (low bit ignored in 32 KB mode)
        mapper1Data_.ramEnable = mapper1Data_.regData[3] & 0x10;

        if (isDebugWrite()) {
          std::cerr << "Cartridge::setROMByte " <<
            std::hex << addr << " " << std::hex << int(c) << "\n";
          std::cerr << "  regData[0] " << std::hex << int(mapper1Data_.regData[0]) << "\n";
          std::cerr << "  regData[1] " << std::hex << int(mapper1Data_.regData[1]) << "\n";
          std::cerr << "  regData[2] " << std::hex << int(mapper1Data_.regData[2]) << "\n";
          std::cerr << "  regData[3] " << std::hex << int(mapper1Data_.regData[3]) << "\n";
        }

        //---

        // reset
        mapper1Data_.regBit   = 0;
        mapper1Data_.regValue = 0;
      }
    }

    return true;
  }
  else if (mapper_ == 2) {
    if      (addr < 0x2000) { // $8000-$9FFF
      mapper2Data_.bank = c & 0x1F;
    }
    else if (addr < 0x4000) { // $A000-$BFFF
    }
    else if (addr < 0x6000) { // $C000-$DFFF
    }
    else if (addr < 0x8000) { // $E000-$FFFF
    }

    return true;
  }
  else {
    if (isDebugWrite())
      std::cerr << "Cartridge::setROMByte : bad mapper " << int(mapper_) << "\n";

    return false;
  }
}

bool
Cartridge::
getVRAMByte(ushort addr, uchar &c) const
{
  c = 0;

  // Pattern Table 0
  if      (addr < 0x1000) {
    if (chrLPage_ >= 0) {
      addr += chrLPage_*0x0400; // 1K

      if (addr >= chrSize_)
        return false;

      c = chrRomData_[addr];
    }
    else {
      if (mapper_ == 1) {
        if (mapper1Data_.mirror == 2)
          addr += mapper1Data_.vromBank[1]*0x1000;
        else
          addr += mapper1Data_.vromBank[0]*0x1000;
      }

      if (addr >= chrSize_)
        return false;

      c = chrRomData_[addr];
    }
  }
  // Pattern Table 1
  else if (addr < 0x2000) {
    ushort addr1 = addr - 0x1000;

    if (chrHPage_ >= 0) {
      addr1 += chrHPage_*0x0400; // 1K

      if (addr1 >= chrSize_)
        return false;

      c = chrRomData_[addr1];
    }
    else {
      if (mapper_ == 1) {
        if (mapper1Data_.mirror == 3)
          addr1 += mapper1Data_.vromBank[0]*0x1000;
        else
          addr1 += mapper1Data_.vromBank[1]*0x1000;
      }

      if (addr1 >= chrSize_)
        return false;

      c = chrRomData_[addr1];
    }
  }

  return true;
}

void
Cartridge::
drawTiles()
{
  int nt = numTiles();

  for (int it = 0; it < nt; ++it)
    drawTile(it);
}

int
Cartridge::
numTiles() const
{
  // 256 tiles 8x8x2 bits
  ushort tileSize = 16*16*8*2;

  return chrSize_/tileSize;
}

void
Cartridge::
drawTile(int it)
{
  currentTile_ = it;

  int ic = 0;

  for (int iy = 0; iy < 16; ++iy) {
    int y = iy*8;

    for (int ix = 0; ix < 16; ++ix, ++ic) {
      int x = ix*8;

      drawTileChar(it, ic, x, y);
    }
  }
}

void
Cartridge::
drawTileChar(int it, int ic, int x, int y)
{
  ushort tileSize = 16*16*8*2;

  ushort p = it*tileSize + ic*16;

  for (int iby = 0; iby < 8; ++iby, ++p) {
    uchar c1 = chrRomData_[p    ];
    uchar c2 = chrRomData_[p + 8];

    for (int ibx = 0; ibx < 8; ++ibx) {
      bool b1 = (c1 & (1 << (7 - ibx)));
      bool b2 = (c2 & (1 << (7 - ibx)));

      setColor(b1 + 2*b2);

      drawPixel(x + ibx, y + iby);
    }
  }
}

}
