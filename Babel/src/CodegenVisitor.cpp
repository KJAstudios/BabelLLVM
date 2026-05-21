#include "Babel/CodegenVisitor.h"
#include "Babel/AbstractSyntaxTree.h"
#include <llvm-18/llvm/ADT/APInt.h>
#include <llvm-18/llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
namespace Babel {
CodegenVisitor::CodegenVisitor(llvm::LLVMContext *context) : context(context) {}
void CodegenVisitor::VisitIntExpression(IntExpressionAST *intExpression) {
  lastResult = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context),
                                      intExpression->GetValue(), true);
}
} // namespace Babel