#ifndef CNES_Machine_H
#define CNES_Machine_H

#include <CNES_Types.h>

namespace CNES {

class CPU;
class PPU;
class Cartridge;

class Machine {
 public:
  Machine();

  virtual ~Machine() { }

  virtual void init();

  CPU       *getCPU () const { return cpu_ ; }
  PPU       *getPPU () const { return ppu_ ; }
  Cartridge *getCart() const { return cart_; }

  bool isDebugRead() const { return debugRead_; }
  void setDebugRead(bool b) { debugRead_ = b; }

  bool isDebugWrite() const { return debugWrite_; }
  void setDebugWrite(bool b) { debugWrite_ = b; }

 protected:
  void initMemory();

  void tick(ushort n);

 protected:
  friend class CPU;

  CPU*       cpu_        { nullptr };
  PPU*       ppu_        { nullptr };
  Cartridge* cart_       { nullptr };
  bool       debugRead_  { false };
  bool       debugWrite_ { false };
};

}

#endif
