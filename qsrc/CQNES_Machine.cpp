#include <CQNES_Machine.h>
#include <CQNES_CPU.h>
#include <CQNES_PPU.h>
#include <CQNES_Cartridge.h>
#include <CQNES_Sprites.h>

namespace CNES {

QMachine::
QMachine() :
 Machine()
{
  setObjectName("Machine");
}

void
QMachine::
init()
{
  cpu_  = qcpu_  = new QCPU      (this);
  ppu_  = qppu_  = new QPPU      (this);
  cart_ = qcart_ = new QCartridge(this);

  sprites_ = new QPPU_Sprites(qppu_);

//connect(qcpu_, SIGNAL(registerChangedSignal()), this, SIGNAL(registerChangedSignal()));

//connect(qcpu_, SIGNAL(flagsChangedSignal()), this, SIGNAL(flagsChangedSignal()));
//connect(qcpu_, SIGNAL(stackChangedSignal()), this, SIGNAL(stackChangedSignal()));

  connect(qcpu_, SIGNAL(pcChangedSignal()), this, SIGNAL(pcChangedSignal()));
  connect(qcpu_, SIGNAL(memChangedSignal(unsigned short, unsigned short)),
          this, SIGNAL(memChangedSignal(unsigned short, unsigned short)));

  connect(qcpu_, SIGNAL(handleBreakSignal()), this, SIGNAL(handleBreakSignal()));
  connect(qcpu_, SIGNAL(breakpointHitSignal()), this, SIGNAL(breakpointHitSignal()));
  connect(qcpu_, SIGNAL(illegalJumpSignal()), this, SIGNAL(illegalJumpSignal()));
  connect(qcpu_, SIGNAL(nmiSignal()), this, SIGNAL(nmiSignal()));

  connect(qcpu_, SIGNAL(breakpointsChangedSignal()), this, SIGNAL(breakpointsChangedSignal()));

  //---

  Machine::init();
}

}
