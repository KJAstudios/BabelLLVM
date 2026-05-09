#ifndef BABEL_LEXER_H
#define BABEL_LEXER_H
#include "Token.h"
#include <string>
namespace Babel{
	class Lexer{
		private:
		std::string currentLine;
		public:
		Babel::Token GetNextToken();
		private: 
		char[] *GetNextCharacter();
		unsigned int GetCharSize();
	};
}
#endif