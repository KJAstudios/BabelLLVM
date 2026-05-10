#ifndef BABEL_LEXER_H
#define BABEL_LEXER_H
#include "Token.h"
#include "llvm/Support/MemoryBuffer.h"
#include <string>
#include <vector>
namespace Babel{
	class Lexer{
		private:
		llvm::MemoryBuffer codeBuffer;
		public:
		Babel::Token GetNextToken();
		void LoadBuffer(std::string *filename = nullptr);
		private: 
		std::vector<char> *GetNextCharacter();
		unsigned int GetCharSize();
	};
}
#endif