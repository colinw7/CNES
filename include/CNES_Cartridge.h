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

  bool load(const std::string &filename);

  bool getROMByte(ushort addr, uchar &c) const;
  bool setROMByte(ushort addr, uchar c);

  virtual void updateState() { }

  int numTiles() const;

  void drawTiles();
  void drawTile(int it);
  void drawTileChar(int it, int ic, int x, int y);

  void drawPPUChar(int it, int x, int y, uchar c);
  void drawPPUCharLine(int it, int x, int y, uchar c, int iby);

  virtual void setColor(uchar /*c*/) { }
  virtual void drawPixel(int /*x*/, int /*y*/) { }

 protected:
  bool loadNES(const std::string &filename);

 protected:
  using Data = std::vector<uchar>;

  Machine *machine_ { nullptr };

  bool  mirroring_    { false };
  bool  batteryRAM_   { false };
  bool  trainer_      { false };
  bool  ignoreMirror_ { false };

  bool  vsUni_      { false };
  bool  playChoice_ { false };
  uchar nesVer_     { 0 };

  uchar mapper_ { 0 };

  ushort prgRamSize_ { 0 };

  uchar tvType_       { 0 };
  bool  hasPrgRam_    { false };
  bool  busConflicts_ { false };

  ushort prgSize_ { 0 };
  Data   prgRomData_;

  ushort chrSize_ { 0 };
  Data   chrRomData_;

  Data trainerData_;

  Data playChoiceData_;
  Data playExtraData_;

  bool resetMapper_;

  uchar register_[4];

  mutable int currentTile_;
};

}

#endif
