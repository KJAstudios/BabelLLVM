#include "Babel/ScopeStack.h"
#include <llvm/IR/Instructions.h>
namespace Babel {
void ScopeStack::PopScope() { scopes.pop_back(); }

void ScopeStack::PushScope() { scopes.emplace_back(); }

void ScopeStack::AddVariable(std::string name, llvm::AllocaInst *value) {
  auto currentScope = scopes.back();
  currentScope[std::move(name)] = value;
}

llvm::AllocaInst *ScopeStack::GetVariableValue(std::string *name) {
  for (auto scope : scopes) {
    auto value = scope.find(*name);
    if (value == scope.end()) {
      continue;
    }
    return value->second;
  }
  return nullptr;
}
} // namespace Babel