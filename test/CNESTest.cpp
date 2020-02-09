#include <CNESTest.h>

using namespace CNES;

int
main(int argc, char **argv)
{
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

  Machine machine;

  machine.init();

  if (debug)
    machine.setDebug(true);

  auto cart = machine.getCart();

  for (const auto &arg : args) {
    if (! cart->load(arg))
      std::cerr << "Failed to load '" << arg << "'\n";
  }

  exit(0);
}
