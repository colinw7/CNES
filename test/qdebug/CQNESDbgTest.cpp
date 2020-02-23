#include <CQNESDbg.h>
#include <CQNES_Machine.h>
#include <CQNES_CPU.h>
#include <CQNES_PPU.h>
#include <CQNES_Cartridge.h>
#include <CQNES_Sprites.h>
#include <CQApp.h>

using namespace CNES;

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  bool debug = false;

  using Args = std::vector<std::string>;

  Args args;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string arg = &argv[i][1];

      if (arg == "D")
        debug = true;
      else {
        std::cerr << "Invalid arg '" << argv[i] << "'\n";
        exit(1);
      }
    }
    else {
      args.push_back(argv[i]);
    }
  }

  //---

  auto machine = new QMachine;

  machine->init();

  if (debug)
    machine->setDebugWrite(true);

  auto file = machine->getCart();

  for (const auto &arg : args) {
    if (! file->load(arg))
      std::cerr << "Failed to load '" << arg << "'\n";
  }

  machine->getQPPU()->show();

//machine->getQCart  ()->show();
//machine->getSprites()->show();

  machine->getCPU()->resetSystem();

  //---

  CQNESDbg *dbg = new CQNESDbg(machine);

  dbg->init();

  dbg->setFixedFont(QFont("Courier New", 16));

  machine->setDbgWidget(dbg);

  //dbg->show();

  return app.exec();
}
