#ifndef CQNESDbg_H
#define CQNESDbg_H

#include <CQ6502Dbg.h>

namespace CNES {

class QMachine;

}

class CQNESDbg : public CQ6502Dbg {
 public:
  CQNESDbg(CNES::QMachine *machine);

 private:
  CNES::QMachine *machine_ { nullptr };
};

#endif
