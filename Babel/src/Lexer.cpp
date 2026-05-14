#include "Babel/Lexer.h"
#include "Babel/Token.h"

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>
#include <memory>
#include <string>
#include <system_error>
namespace Babel {
void Lexer::LoadBuffer(std::string *filename) {
  if (filename != nullptr) {
    TryLoadCodeIntoBuffer(llvm::MemoryBuffer::getFile(*filename));
  }

  TryLoadCodeIntoBuffer(llvm::MemoryBuffer::getSTDIN());
}

void Lexer::TryLoadCodeIntoBuffer(
    const llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> memoryBuffer) {
  if (!memoryBuffer) {
    std::error_code error = memoryBuffer.getError();
    std::cerr << error;
    return;
  }

  stringCodeBuffer = memoryBuffer.get()->getBuffer();
}

Babel::Token Lexer::GetNextToken() {
  if (stringCodeBuffer.empty()) {
    std::cerr << "Buffer is empty\n";
    return Token::tok_eof;
  }

  offset = 0;

  bool isTokenEnd = false;

  while (!isTokenEnd && offset < stringCodeBuffer.size()) {
    unsigned charSize = GetCharSize(stringCodeBuffer[offset]);
    llvm::StringRef nextChar =
        llvm::StringRef(stringCodeBuffer.data() + offset, charSize);

    // if it's a comment, skip the entire line
    if (nextChar.str() == "✎") {
      SkipComment();
      continue;
    }

    // if it starts with a number, it should be a number token
    if (charSize == 1 && (isdigit(stringCodeBuffer[offset]) != 0)) {
      return LexNumberToken();
    }

    // if the char size is 1 and it's whitespace, the token has ended
    if (charSize == 1 && IsWhitespaceCharacter(stringCodeBuffer[offset])) {
      return LexWhitespaceTerminatedToken(charSize);
    }

    if (IsControlCharacter(nextChar)) {
     return LexControlCharTerminatedToken(charSize, nextChar);
    }

    if (IsOperatorCharacter(nextChar)) {
      return LexOperatorTerminatedToken(charSize,  nextChar);
    }

    // move to the next character
    offset += charSize;
  }

  // if we reach the end of the buffer, we're at the end of the file/input
  std::cerr << "End of file reached\n";
  return Token::tok_eof;
}

void Lexer::SkipComment() {
  while (offset < stringCodeBuffer.size() && stringCodeBuffer[offset] != '\n') {

    offset += GetCharSize(stringCodeBuffer[offset]);
  }
}

Babel::Token Lexer::LexNumberToken() {

  int dotCount = 0;
  char curChar = stringCodeBuffer[offset];

  while (offset < stringCodeBuffer.size()) {

    // if the next character isn't ascii, we need to check if it's a valid
    // unicode operator or control. We also need to check if it's a '~'
    // character, which is the only valid ascii control or operator character
    if (isascii(curChar) == 0 || curChar == '~') {
      unsigned charSize = GetCharSize(curChar);
      llvm::StringRef unicodeChar =
          llvm::StringRef(stringCodeBuffer.data() + offset, charSize);

      if (IsControlCharacter(unicodeChar) || IsOperatorCharacter(unicodeChar)) {
        // valid end of number, return a number token
        numberStr = llvm::StringRef(stringCodeBuffer.data(), offset);
        stringCodeBuffer = stringCodeBuffer.drop_front(offset + 1);
        return Token::tok_number;
      }

      std::cerr << "Invalid character in number\n";
      return Token::tok_error;
    }

    // if we encounter whitespace, the number token has ended
    if (IsWhitespaceCharacter(curChar)) {
      numberStr = llvm::StringRef(stringCodeBuffer.data(), offset);
      stringCodeBuffer = stringCodeBuffer.drop_front(offset + 1);
      return Token::tok_number;
    }

    // if we encounter a dot, it's either a decimal point or an invalid
    // character in the number
    if (curChar == '.') {
      if (dotCount > 1) {
        std::cerr << "Invalid number format\n";
        return Token::tok_error;
      }

      dotCount++;
      offset++;
      continue;
    }

    // if the character isn't a digit, it's an invalid character in the number
    if (isdigit(curChar) == 0) {
      std::cerr << "Invalid character in number\n";
      return Token::tok_error;
    }

    // if it's a valid digit, move to the next character
    offset++;
  }
  // if the end of the buffer is reached, that means the file ended without a
  // valid end to the number token
  return Token::tok_eof;
}

Babel::Token Lexer::LexWhitespaceTerminatedToken(unsigned charSize) {
  bool isWhitespaceCharacter = IsWhitespaceCharacter(stringCodeBuffer[offset]);
  if (offset == 0) {
    if (isWhitespaceCharacter) {
      stringCodeBuffer = stringCodeBuffer.drop_front(1);
      return GetNextToken();
    }
  }

  if (isWhitespaceCharacter) {
    llvm::StringRef token = llvm::StringRef(stringCodeBuffer.data(), offset);
    stringCodeBuffer = stringCodeBuffer.drop_front(offset + charSize);
    return GetTokIdentifierOrKeyword(token);
  }

  std::cerr << "Invalid whitespace character\n";
  return Token::tok_error;
}

Babel::Token Lexer::LexControlCharTerminatedToken(unsigned charSize,
                                                  llvm::StringRef nextChar) {
  // if the first character is a control character, it's a control token
  if (offset == 0) {
    controlCharacter = nextChar;
    stringCodeBuffer = stringCodeBuffer.drop_front(charSize);
    return Token::tok_control;
  }

  // else it's an identifier found before the control character
  llvm::StringRef token = llvm::StringRef(stringCodeBuffer.data(), offset);
  stringCodeBuffer = stringCodeBuffer.drop_front(offset + charSize);
  return GetTokIdentifierOrKeyword(token);
}

Babel::Token Lexer::LexOperatorTerminatedToken(unsigned charSize,
                                               llvm::StringRef nextChar) {
  // if the first character is an operator, it's an operator token
  if (offset == 0) {
    operatorStr = nextChar;
    stringCodeBuffer = stringCodeBuffer.drop_front(charSize);
    return Token::tok_operator;
  }

  // else it's an identifier found before the operator
  llvm::StringRef token = llvm::StringRef(stringCodeBuffer.data(), offset);
  stringCodeBuffer = stringCodeBuffer.drop_front(offset + charSize);
  return GetTokIdentifierOrKeyword(token);
}

Babel::Token Lexer::GetTokIdentifierOrKeyword(llvm::StringRef identifier) {

  if (identifier.str() == "əgər") {
    return Token::tok_if;
  }
  if (identifier.str() == "それ以外") {
    return Token::tok_else;
  }
  if (identifier.str() == "tisk") {
    return Token::tok_print;
  }
  if (identifier.str() == "ለ") {
    return Token::tok_for;
  }
  if (identifier.str() == "yekjimar") {
    return Token::tok_int;
  }
  if (identifier.str() == "kaksinkertainen") {
    return Token::tok_double;
  }

  identifierStr = identifier;
  return Token::tok_identifier;
}

// Get the size of the unicode character at the current pointer
unsigned int Lexer::GetCharSize(const char pointer) {
  auto value = static_cast<uint8_t>(pointer);
  if (value < 0x80) {
    return 1;
  }
  if (value < 0xE0) {
    return 2;
  }
  if (value < 0xF0) {
    return 3;
  }
  return 4;
}

bool Lexer::IsControlCharacter(llvm::StringRef character) {
  return character.str() == "~" || character.str() == "꧁" ||
         character.str() == "꧂" || character.str() == "⟅" ||
         character.str() == "⟆";
}

bool Lexer::IsWhitespaceCharacter(char character) {
  switch (character) {
  case ' ':
  case '\t':
  case '\n':
  case '\r':
    return true;
  default:
    return false;
  }
}

bool Lexer::IsOperatorCharacter(llvm::StringRef character) {
  if (character.str() == "⊕") {
    return true;
  }
  if (character.str() == "⊖") {
    return true;
  }
  if (character.str() == "×") {
    return true;
  }
  if (character.str() == "÷") {
    return true;
  }
  if (character.str() == "≔") {
    return true;
  }
  if (character.str() == "≺") {
    return true;
  }
  if (character.str() == "≻") {
    return true;
  }

  return false;
}
} // namespace Babel