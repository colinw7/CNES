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

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

 protected:
  void initMemory();

  void tick(ushort n);

 protected:
  friend class CPU;

  CPU*       cpu_   { nullptr };
  PPU*       ppu_   { nullptr };
  Cartridge* cart_  { nullptr };
  bool       debug_ { false };
};

}

#endif
