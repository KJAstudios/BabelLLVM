#include "Babel/DebugInfo.h"
#include <llvm-20/llvm/ADT/StringRef.h>
#include <llvm-20/llvm/BinaryFormat/Dwarf.h>
#include <llvm-20/llvm/IR/DIBuilder.h>
#include <llvm-20/llvm/IR/DebugInfoMetadata.h>

namespace Babel {

DebugInfo::DebugInfo(std::string &fileName)
    : fileName(std::move(fileName)),
      compileUnit(dwarfBuilder->createCompileUnit(
          BabelLanguageCode, dwarfBuilder->createFile(fileName, "."),
          "Babel Compiler", false, "", 0)),
      Unit(dwarfBuilder->createFile(compileUnit->getFilename(),
                                    compileUnit->getDirectory())) {}

void DebugInfo::CreateFunction(std::string &functionName, llvm::Function *function) {
  llvm::DIScope *fileContext = Unit;
  unsigned lineNumber = 0;
  unsigned scopeLine = 0;
  llvm::DISubprogram *subProgram = dwarfBuilder->createFunction(
      fileContext, functionName, llvm::StringRef(), Unit, lineNumber,
      CreateFunctionType(function->arg_size()), scopeLine, llvm::DINode::FlagPrototyped,
      llvm::DISubprogram::SPFlagDefinition);
      function->setSubprogram(subProgram);
}

llvm::DIType *DebugInfo::GetIntType() {
  if (intType != nullptr) {
    return intType;
  }

  intType =
      dwarfBuilder->createBasicType("int", 64, llvm::dwarf::DW_ATE_signed);
  return intType;
}

llvm::DIBuilder *DebugInfo::GetDwarfBuilder() { return dwarfBuilder.get(); }

llvm::DISubroutineType *DebugInfo::CreateFunctionType(unsigned NumArgs) {
  llvm::SmallVector<llvm::Metadata *, 8> elementTypes;
  llvm::DIType *intType = GetIntType();

  elementTypes.push_back(intType);

  for (unsigned i = 0, e = NumArgs; i != e; ++i) {
    elementTypes.push_back(intType);
  }

  return dwarfBuilder->createSubroutineType(
      dwarfBuilder->getOrCreateTypeArray(elementTypes));
}
} // namespace Babel