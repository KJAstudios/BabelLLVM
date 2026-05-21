#include "Babel/Parser.h"

int main(int argCount, char *argValues[]) {
  if (argCount > 1) {
    // invalid file mode
    return 1;
  }

  Babel::Parser parser = Babel::Parser(nullptr);
  parser.Parse();
}