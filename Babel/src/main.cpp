#include "Babel/Lexer.h"
#include "Babel/Parser.h"
#include "Babel/Token.h"
#include <iostream>

int main(int argCount, char *argValues[]) {
  if (argCount > 1) {
    // invalid file mode
    return 1;
  }

  Babel::Parser parser = Babel::Parser(nullptr);
  parser.Parse();
}