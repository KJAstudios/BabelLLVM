#include "Babel/Parser.h"
#include <memory>
namespace Babel {
Babel::Parser::Parser(std::string *filename) : tokenState(Token::tok_empty) {
  lexer = std::make_unique<Lexer>();
  lexer->LoadBuffer(filename);
}
Babel::Token Babel::Parser::GetNextToken() {
  tokenState = lexer->GetNextToken();
  return tokenState;
}
} // namespace Babel