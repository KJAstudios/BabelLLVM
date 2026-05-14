#include "Babel/Lexer.h"
#include "Babel/Token.h"
#include <iostream>

int main(int argCount, char *argValues[]) {
  if (argCount > 1) {
    // invalid file mode
    return 1;
  }

  Babel::Lexer lexer = Babel::Lexer();
  lexer.LoadBuffer();
  Babel::Token token = lexer.GetNextToken();
  while (token != Babel::Token::tok_eof) {
    token = lexer.GetNextToken();
  }
}