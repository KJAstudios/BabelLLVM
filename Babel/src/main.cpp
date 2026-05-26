#include "Babel/Babel.h"

namespace Babel {

int main(int argCount, char *argValues[]) {
  if (argCount > 1) {
    // invalid file mode
    return 1;
  }

  Babel babel = Babel();
  babel.Run();
};
} // namespace Babel