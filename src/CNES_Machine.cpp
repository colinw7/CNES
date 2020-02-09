#include <CNES_Machine.h>
#include <CNES_CPU.h>
#include <CNES_PPU.h>
#include <CNES_Cartridge.h>

#include <cassert>

namespace CNES {

Machine::
Machine()
{
}

void
Machine::
init()
{
  if (! cpu_)
    cpu_ = new CPU(this);

  if (! ppu_)
    ppu_ = new PPU(this);

  if (! cart_)
    cart_ = new Cartridge(this);

  initMemory();

  // call 6502 reset vector
  cpu_->resetSystem();
}

void
Machine::
initMemory()
{
}

void
Machine::
tick(ushort /*n*/)
{
}

}
