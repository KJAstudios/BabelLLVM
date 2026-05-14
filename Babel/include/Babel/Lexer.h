#ifndef BABEL_LEXER_H
#define BABEL_LEXER_H
#include "Token.h"
#include "llvm/Support/MemoryBuffer.h"
#include <llvm-18/llvm/ADT/StringRef.h>
#include <memory>
#include <string>
#include <vector>

namespace Babel {
class Lexer {
private:
  llvm::StringRef stringCodeBuffer;
  llvm::StringRef identifierStr;
  llvm::StringRef operatorStr;
  llvm::StringRef controlCharacter;

public:
  Babel::Token GetNextToken();
  void LoadBuffer(std::string *filename = nullptr);

private:
  unsigned int GetCharSize(const char pointer);
  void TryLoadCodeIntoBuffer(
      llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> memoryBuffer);
  bool IsControlCharacter(llvm::StringRef character);
  bool IsOperatorCharacter(llvm::StringRef character);
  bool IsWhitespaceCharacter(char character);
  llvm::StringRef *GetIdentifierStr() { return &identifierStr; }
  Babel::Token GetTokIdentifierOrKeyword(llvm::StringRef identifier);
};
} // namespace Babel
#endif