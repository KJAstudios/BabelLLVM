#ifndef SCOPESTACK_H
#define SCOPESTACK_H
#include <llvm-18/llvm/IR/Instructions.h>
#include <llvm-18/llvm/IR/Value.h>
#include <map>
#include <string>
#include <vector>

namespace Babel {
class ScopeStack {
private:
  std::vector<std::map<std::string, llvm::AllocaInst*>> ScopeStack;

public:
	void PopScope();
	void PushScope();
	void AddVariable(std::string name, llvm::AllocaInst *value);
	llvm::AllocaInst* GetVariableValue(std::string *name);
};
} // namespace Babel
#endif