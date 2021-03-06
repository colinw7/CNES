#ifndef CNES_Cartridge_H
#define CNES_Cartridge_H

#include <CNES_Types.h>
#include <string>
#include <vector>

namespace CNES {

class Machine;

class Cartridge {
 public:
  Cartridge(Machine *machine);

  virtual ~Cartridge() { }

  bool isDebugRead() const { return debugRead_; }
  void setDebugRead(bool b) { debugRead_ = b; }

  bool isDebugWrite() const { return debugWrite_; }
  void setDebugWrite(bool b) { debugWrite_ = b; }

  bool load(const std::string &filename);

  ushort prgSize() const { return prgSize_; }
  ushort chrSize() const { return chrSize_; }

  bool isMirroring() const { return mirroring_; }

  uchar mapper() const { return mapper_; }

  ushort prgRamSize() const { return prgRamSize_; }

  int chrLPage() const { return chrLPage_; }
  void setChrLPage(int i) { chrLPage_ = i; }

  int chrHPage() const { return chrHPage_; }
  void setChrHPage(int i) { chrHPage_ = i; }

  bool getLowerROMByte(ushort addr, uchar &c) const;
  bool getUpperROMByte(ushort addr, uchar &c) const;

  bool setROMByte(ushort addr, uchar c);

  bool getVRAMByte(ushort addr, uchar &c) const;

  virtual void updateState() { }

  int numTiles() const;

  void drawTiles();
  void drawTile(int it);
  void drawTileChar(int it, int ic, int x, int y);

//void drawPPUChar(int x, int y, uchar c);
//void drawPPUCharLine(int x, int y, uchar c, int iby, uchar ac);

//void drawSpriteChar(int x, int y, uchar c, bool doubleHeight);
//void drawSpriteCharLine(int x, int y, uchar c, int iby, uchar ac);

  virtual void setColor(uchar /*c*/) { }
  virtual void drawPixel(int /*x*/, int /*y*/) { }

 protected:
  bool loadNES(const std::string &filename);

 protected:
  using Data = std::vector<uchar>;

  Machine *machine_ { nullptr };

  bool debugRead_  { false };
  bool debugWrite_ { false };

  bool  mirroring_    { false };
  bool  batteryRAM_   { false };
  bool  trainer_      { false };
  bool  ignoreMirror_ { false };

  uchar consoleType_ { 0 };
  uchar nesVer_      { 0 };

  uchar mapper_ { 0 };

  uchar tvType_ { 0 };

  bool   hasPrgRam_  { false };
  uchar  prgCount_   { 0 };
  ushort prgRamSize_ { 0 };
  Data   prgRamData_;

  bool  busConflicts_ { false };

  uchar  romCount_ { 0 };
  ushort prgSize_  { 0 };
  Data   prgRomData_;

  uchar  chrCount_ { 0 };
  ushort chrSize_  { 0 };
  Data   chrRomData_;

  Data trainerData_;

  Data playChoiceData_;
  Data playExtraData_;

  struct Mapper1Data {
    bool resetMapper { false };

    uchar regData[4];

    uchar regBit   { 0 };
    uchar regValue { 0 };

    uchar mirror    { 0 };

    uchar romBank   { 1 };
    uchar romSize   { 0 };

    uchar vromMode    { 0 };
    uchar vromBank[2] { };

    uchar romPage   { 0 };
    bool  ramEnable { false };
  };

  struct Mapper2Data {
    uchar bank { 0 };
  };

  struct Ver2Data {
    uchar ppuType1  { 0x00 };
    uchar hardType1 { 0x00 };
    uchar extType   { 0x00 };
    uchar prgShift2 { 0x00 };
    uchar defExp    { 0x00 };
    uchar miscRoms  { 0x00 };
    uchar subMapper { 0x00 };
    uchar prgSize1  { 0x00 };
    uchar mapperHi1 { 0x00 };
    uchar chrSize1  { 0x00 };
    uchar ppuTiming { 0x00 };
    uchar prgShift1 { 0x00 };
    uchar chrRam1   { 0x00 };
    uchar chrRam2   { 0x00 };
  };

  Mapper1Data mapper1Data_;
  Mapper2Data mapper2Data_;
  Ver2Data    ver2Data;

  int chrLPage_ { -1 };
  int chrHPage_ { -1 };

  mutable int currentTile_;
};

}

#endif
