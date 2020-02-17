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

  vsUni_      = header[7] & 0x01;        // 1 for VS-System cartridges
  playChoice_ = header[7] & 0x02;
  nesVer_     = (header[7] & 0x0C) >> 2;

  uchar mapperHi = (header[7] & 0xF0) >> 4; // Four higher bits of ROM Mapper Type.

  prgCount_   = (header[8] ? header[8] : 1); // Number of 8kB RAM banks
  prgRamSize_ = prgCount_*8192;              // RAM size

  tvType_       = header[9] & 0x03;     // 1 for PAL cartridges, otherwise assume NTSC
  hasPrgRam_    = ! (header[9] & 0x10);
  busConflicts_ = header[9] & 0x20;

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
    (void) readData(prgRamData_, prgRamSize_);
  }

  //---

  if (playChoice_) {
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

        // 0: one-screen, lower bank
        // 1: one-screen, upper bank
        // 2: vertical
        // 3: horizontal
        mapper1Data_.mirror = mapper1Data_.regData[0] & 0x03;

        // PRG ROM bank mode
        //  0, 1: switch 32 KB at $8000,
        //        ignoring low bit of bank number
        //  2:    fix first bank at $8000 and
        //        switch 16 KB bank at $C000
        //  3:    fix last bank at $C000 and
        //        switch 16 KB bank at $8000
        mapper1Data_.romMode   = mapper1Data_.regData[0] & 0x0C;

        // CHR ROM bank mode
        //  0: switch 8 KB at a time
        //  1: switch two separate 4 KB banks
        mapper1Data_.vromMode  = mapper1Data_.regData[0] & 0x10;

        // Select 4 KB or 8 KB CHR bank at PPU $0000
        // (low bit ignored in 8 KB mode)
        mapper1Data_.vromBank0 = mapper1Data_.regData[1] & 0x1F;

        // Select 4 KB CHR bank at PPU $1000
        // (ignored in 8 KB mode)
        mapper1Data_.vromBank1 = mapper1Data_.regData[2] & 0x1F;

        // Select 16 KB PRG ROM bank
        // (low bit ignored in 32 KB mode)
        mapper1Data_.romPage   = mapper1Data_.regData[3] & 0x0F;

        // PRG RAM chip enable
        // (0: enabled; 1: disabled; ignored on MMC1A)
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
    if (addr >= chrSize_)
      return false;

    c = chrRomData_[addr];
  }
  // Pattern Table 1
  else if (addr < 0x2000) {
    addr += 0x2000;

    if (addr >= chrSize_)
      return false;

    c = chrRomData_[addr];
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

//ushort tileWidth  = 16*8;
//ushort tileHeight = 16*8;

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

#if 0
void
Cartridge::
drawPPUChar(int x, int y, uchar c)
{
  for (int iby = 0; iby < 8; ++iby)
    drawPPUCharLine(x, y, c, iby, 0);
}
#endif

#if 0
void
Cartridge::
drawPPUCharLine(int x, int y, uchar c, int iby, uchar ac)
{
  auto cpu = machine_->getCPU();
  auto ppu = machine_->getPPU();

//ushort tileSize = 16*16*8*2;

  ushort p = cpu->screenPatternAddr() + c*16 + iby;

  uchar c1; getVRAMByte(p    , c1); // color bit 0
  uchar c2; getVRAMByte(p + 8, c2); // color bit 1

  for (int ibx = 0; ibx < 8; ++ibx) {
    bool b1 = (c1 & (1 << (7 - ibx)));
    bool b2 = (c2 & (1 << (7 - ibx)));

    uchar color = ppu->palette((b1 | (b2 << 1)) | ac);

    ppu->setColor(color);

    ppu->drawPixel(x + ibx, y + iby);
  }
}
#endif

#if 0
void
Cartridge::
drawSpriteChar(int x, int y, uchar c, bool doubleHeight)
{
  auto cpu = machine_->getCPU();
  auto ppu = machine_->getPPU();

  for (int iby = 0; iby < 8; ++iby) {
    ushort p = cpu->spritePatternAddr() + c*16 + iby;

    uchar c1; getVRAMByte(p    , c1);
    uchar c2; getVRAMByte(p + 8, c2);

    for (int ibx = 0; ibx < 8; ++ibx) {
      bool b1 = (c1 & (1 << (7 - ibx)));
      bool b2 = (c2 & (1 << (7 - ibx)));

      ppu->setColor(b1 + 2*b2);

      ppu->drawPixel(x + ibx, y + iby);
    }
  }

  if (doubleHeight) {
    y += 8;

    for (int iby = 0; iby < 8; ++iby) {
      ushort p = cpu->spritePatternAltAddr() + c*16 + iby;

      uchar c1; getVRAMByte(p    , c1);
      uchar c2; getVRAMByte(p + 8, c2);

      for (int ibx = 0; ibx < 8; ++ibx) {
        bool b1 = (c1 & (1 << (7 - ibx)));
        bool b2 = (c2 & (1 << (7 - ibx)));

        ppu->setColor(b1 + 2*b2);

        ppu->drawPixel(x + ibx, y + iby);
      }
    }
  }
}
#endif

#if 0
// draw nth sprite line (iby) for sprite at (x, y) and character (c) and
// bits 2 and 3 (ac)
void
Cartridge::
drawSpriteCharLine(int x, int y, uchar c, int iby, uchar ac)
{
  auto cpu = machine_->getCPU();
  auto ppu = machine_->getPPU();

  ushort p;

  if (iby < 8)
    p = cpu->spritePatternAddr() + c*16 + iby;
  else
    p = cpu->spritePatternAltAddr() + c*16 + iby - 8;

  uchar c1; getVRAMByte(p    , c1);
  uchar c2; getVRAMByte(p + 8, c2);

  int x1 = x;
  int y1 = y + iby;

  for (int ibx = 0; ibx < 8; ++ibx, ++x1) {
    // get color bits 0 and 1 from sprite pattern
    bool b1 = (c1 & (1 << (7 - ibx)));
    bool b2 = (c2 & (1 << (7 - ibx)));

    uchar color = ppu->spritePalette((b1 | (b2 << 1)) | ac);

    ppu->setColor(color);

    if (x1 >= 0 && x1 < 256)
      ppu->drawPixel(x1, y1);
  }
}
#endif

}
