#ifndef TOKENDATA_H
#define TOKENDATA_H
#include "Babel/Token.h"
#include <llvm-20/llvm/ADT/StringRef.h>
namespace Babel {
struct TokenLocation {
private:
  int line;
  int column;

public:
  TokenLocation() : line(0), column(0) {}
  TokenLocation(int line, int column) : line(line), column(column) {}
  int GetLine() const { return line; }
  int GetColumn() const { return column; }
};
struct TokenData {
private:
  llvm::StringRef tokenString;
  Token tokenType;
  TokenLocation tokenLocation;

public:
  TokenData(llvm::StringRef tokenString, Token tokenType, int line = 0,
            int column = 0);
  Token GetTokenType();
  std::string GetTokenString();
  TokenLocation GetTokenLocation();
};
} // namespace Babel
#endif