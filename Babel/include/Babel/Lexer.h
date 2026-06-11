#ifndef BABEL_LEXER_H
#define BABEL_LEXER_H
#include "Babel/TokenData.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>
#include <memory>
#include <string>


namespace Babel {
class Lexer {
private:
  // core input buffer
  std::unique_ptr<llvm::MemoryBuffer> rawMemoryBuffer;
  llvm::StringRef stringCodeBuffer;

  // debug location information
  int line = 1;
  int column = 0;

  // helper control variables
  unsigned offset;

public:
  Babel::TokenData GetNextToken();
  void LoadBuffer(std::string *filename = nullptr);

private:
  void ReadFromSTDIN();
  // core functions for the token parsing loop
  void SkipComment();
  Babel::TokenData LexNumberToken();
  Babel::TokenData LexWhitespaceTerminatedToken(unsigned charSize);
  Babel::TokenData LexControlCharTerminatedToken(unsigned charSize,
                                                 llvm::StringRef nextChar);
  Babel::TokenData LexOperatorTerminatedToken(unsigned charSize,
                                              llvm::StringRef nextChar);
  Babel::TokenData GetTokIdentifierOrKeyword(llvm::StringRef identifier);

  // lexing helper functions
  unsigned int GetCharSize(char pointer);
  bool IsControlCharacter(llvm::StringRef character);
  bool IsOperatorCharacter(llvm::StringRef character);
  bool IsWhitespaceCharacter(char character);
};

} // namespace Babel
#endif