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

  prgSize_ = header[4]*16384; // Number of 16kB ROM banks.
  chrSize_ = header[5]*8192;  // Number of 8kB VROM banks

  mirroring_    = header[6] & 0x01; // 1 for vertical mirroring, 0 for horizontal mirroring.
  batteryRAM_   = header[6] & 0x02; // 1 for battery-backed RAM at $6000-$7FFF
  trainer_      = header[6] & 0x04; // 1 for a 512-byte trainer at $7000-$71FF
  ignoreMirror_ = header[6] & 0x08; // 1 for a four-screen VRAM layout

  uchar mapperLo = (header[6] & 0xF0) >> 4; // Four lower bits of ROM Mapper Type

  vsUni_      = header[7] & 0x01;        // 1 for VS-System cartridges
  playChoice_ = header[7] & 0x02;
  nesVer_     = (header[7] & 0x0C) >> 2;

  uchar mapperHi = (header[7] & 0xF0) >> 4; // Four higher bits of ROM Mapper Type.

  prgRamSize_ = (header[8] ? header[8]*8192 : 8192); // Number of 8kB RAM banks

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

  // character data
  if (! readData(chrRomData_, chrSize_))
    return false;

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
getROMByte(ushort addr, uchar &c) const
{
  if (addr >= prgSize_)
    return false;

  c = prgRomData_[addr];

  return true;
}

bool
Cartridge::
setROMByte(ushort addr, uchar c)
{
  if (mapper_ == 1) {
    int regNum = 0;

    if      (addr < 0x2000) regNum = 0;
    else if (addr < 0x4000) regNum = 1;
    else if (addr < 0x6000) regNum = 2;
    else if (addr < 0x8000) regNum = 3;
    else                    return false;

    if      (resetMapper_)
      register_[regNum] = c;
    else if (c == 0x80)
      resetMapper_ = true;

    return true;
  }
  else {
    return false;
  }
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

void
Cartridge::
drawPPUChar(int it, int x, int y, uchar c)
{
  for (int iby = 0; iby < 8; ++iby)
    drawPPUCharLine(it, x, y, c, iby);
}

void
Cartridge::
drawPPUCharLine(int it, int x, int y, uchar c, int iby)
{
  auto cpu = machine_->getCPU();
  auto ppu = machine_->getPPU();

  ushort tileSize = 16*16*8*2;

  ushort p = cpu->screenPatternAddr() + it*tileSize + c*16 + iby;

  uchar c1 = chrRomData_[p    ];
  uchar c2 = chrRomData_[p + 8];

  for (int ibx = 0; ibx < 8; ++ibx) {
    bool b1 = (c1 & (1 << (7 - ibx)));
    bool b2 = (c2 & (1 << (7 - ibx)));

    ppu->setColor(b1 + 2*b2);

    ppu->drawPixel(x + ibx, y + iby);
  }
}

}
