#ifndef PARSER_H
#define PARSER_H
#include "Babel/Lexer.h"
#include "Babel/Token.h"
#include <memory>
namespace Babel {
class Parser {
private:
  std::unique_ptr<Lexer> lexer;
  Token tokenState;

public:
  Parser() = delete;
  explicit Parser(std::string *filename = nullptr);

  private:
  Babel::Token GetNextToken();
};
} // namespace Babel
#endif