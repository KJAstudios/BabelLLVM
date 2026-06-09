#ifndef TOKENDATA_H
#define TOKENDATA_H
#include "Babel/Token.h"
#include <llvm-20/llvm/ADT/StringRef.h>
namespace Babel {
struct TokenData {
private:
  llvm::StringRef tokenString;
  Token tokenType;
  int line;
  int column;

public:
  TokenData(llvm::StringRef tokenString, Token tokenType, int line = 0,
            int column = 0);
  Token GetTokenType();
  std::string GetTokenString();
};
} // namespace Babel
#endif