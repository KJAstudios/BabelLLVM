#include "Babel/Babel.h"



int main(int argCount, char *argv[]) {
  if (argCount > 1) {
    // invalid file mode
    return 1;
  }

  Babel::Babel babel = Babel::Babel();
  babel.Run();
};
