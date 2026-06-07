#include "Babel/DebugInfo.h"
#include <llvm-20/llvm/BinaryFormat/Dwarf.h>
#include <llvm-20/llvm/IR/DIBuilder.h>
#include <llvm-20/llvm/IR/DebugInfoMetadata.h>

namespace Babel {

DebugInfo::DebugInfo(std::string &fileName)
    : fileName(std::move(fileName)),
      compileUnit(dwarfBuilder->createCompileUnit(
          BabelLanguageCode, dwarfBuilder->createFile(fileName, "."),
          "Babel Compiler", false, "", 0)) {}

llvm::DIType *DebugInfo::GetIntType() {
  if (intType != nullptr) {
    return intType;
  }

  intType =
      dwarfBuilder->createBasicType("int", 64, llvm::dwarf::DW_ATE_signed);
  return intType;
}

llvm::DIBuilder *DebugInfo::GetDwarfBuilder() { return dwarfBuilder.get(); }
} // namespace Babel