#include <CQNES_CPU.h>
#include <CQNES_Machine.h>

namespace CNES {

QCPU::
QCPU(QMachine *machine) :
 CPU(machine)
{
  setObjectName("6502");
}

}
