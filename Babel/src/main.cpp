#include "Babel/Lexer.h"

int main(int argCount, char* argValues[]){
	if(argCount > 1){
		// file mode
		return 1;
	}

	Babel::Lexer lexer = Babel::Lexer();
	lexer.GetNextToken();
}