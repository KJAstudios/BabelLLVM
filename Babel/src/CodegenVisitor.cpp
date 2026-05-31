#include "Babel/CodegenVisitor.h"
#include "Babel/AbstractSyntaxTree.h"
#include "Babel/ScopeStack.h"
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <memory>
#include <vector>

namespace Babel {
CodegenVisitor::CodegenVisitor(llvm::LLVMContext *context,
                               llvm::IRBuilder<> *builder, llvm::Module *module)
    : context(context), builder(builder), module(module) {
  scopeStack = std::make_unique<ScopeStack>();

  // map builtin functions
  builtinFunctionMap["tisk"] = "print";
}

llvm::AllocaInst *
CodegenVisitor::CreateEntryBlockAlloca(llvm::Function *function,
                                       std::string variableName) {
  llvm::IRBuilder<> tempBuilder(&function->getEntryBlock(),
                                function->getEntryBlock().begin());
  return tempBuilder.CreateAlloca(llvm::Type::getInt64Ty(*context), nullptr,
                                  variableName);
}

void CodegenVisitor::VisitProgram(ProgramAST *program) {
  std::cerr << "Found " << program->GetFunctions()->size()
            << " functions in the program\n";
  for (const auto &function : *program->GetFunctions()) {
    std::cerr << function->GetPrototype()->GetName() << " found\n";
    function->GetPrototype()->Visit(*this);
    if (statementGenerationFailed) {
      std::cerr << "Error generating function definitions";
      return;
    }
  }

  std::cerr << "Found " << program->GetFunctions()->size()
            << " functions in the program\n";
  for (const auto &function : *program->GetFunctions()) {
    std::cerr << function->GetPrototype()->GetName() << " found\n";
    function->Visit(*this);
    if (statementGenerationFailed) {
      std::cerr << "Error generating functions";
      return;
    }
  }
}

void CodegenVisitor::VisitPrototype(PrototypeAST *prototype) {
  // make a list of the function paramaters with type
  const std::vector<llvm::Type *> argumentTypeList(
      prototype->GetArgs()->size(), llvm::Type::getInt64Ty(*context));

  // build a function type with the given arguments and the return type
  llvm::FunctionType *functionType = llvm::FunctionType::get(
      llvm::Type::getInt64Ty(*context), argumentTypeList, false);

  // finally create the function itself
  llvm::Function *function =
      llvm::Function::Create(functionType, llvm::Function::ExternalLinkage,
                             *prototype->GetName(), module);
  // and then connect the arguments to the correct name
  unsigned index = 0;
  const auto *prototypeArgs = prototype->GetArgs();
  for (auto &Arg : function->args()) {
    Arg.setName(prototypeArgs->at(index++));
  }
}

void CodegenVisitor::VisitFunction(FunctionAST *function) {
  PrototypeAST *prototype = function->GetPrototype();
  if (prototype == nullptr) {
    statementGenerationFailed = true;
    return;
  }
  std::cerr << "Function Prototype found for function generation\n";
  std::string *prototypeName = prototype->GetName();

  // TODO handle extern functions
  // check if the function already exists
  llvm::Function *llvmFunction = module->getFunction(*prototypeName);

  // if the function already exists, don't redefine it
  if (llvmFunction == nullptr) {
    std::cerr << "Function does not exist\n";
    statementGenerationFailed = true;
    return;
  }

  llvmFunction = module->getFunction(*prototypeName);
  if (llvmFunction == nullptr) {
    std::cerr << "Function prototype not found\n";
    statementGenerationFailed = true;
    return;
  }

  std::cerr << "Function found in module\n";
  // create the basic block for the function
  llvm::BasicBlock *block =
      llvm::BasicBlock::Create(*context, "entry", llvmFunction);
  // make sure the builder is inserting into the function
  builder->SetInsertPoint(block);

  // add scope for the function variables
  scopeStack->PushScope();
  // create the function variables and add them to scope
  for (auto &Arg : llvmFunction->args()) {
    llvm::AllocaInst *alloca =
        CreateEntryBlockAlloca(llvmFunction, Arg.getName().str());
    builder->CreateStore(&Arg, alloca);
    scopeStack->AddVariable(Arg.getName().str(), alloca);
  }

  std::cerr << "generating function body\n";
  function->GetBody()->Visit(*this);
  if (statementGenerationFailed) {
    return;
  }

  // TODO function verification
  builder->CreateRet(
      llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0, true));

  // remove the function scope
  scopeStack->PopScope();
}

void CodegenVisitor::VisitFunctionCallStatement(
    FunctionCallStatementAST *functionCall) {

  std::string functionName = *functionCall->GetName();
  std::string builtInName;
  if (TryGetBuiltInFunctionName(&functionName,
                                llvm::IntegerType::getInt64Ty(*context),
                                builtInName)) {
    functionName = builtInName;
  }
  llvm::Function *function = FindFunction(&functionName);
  if (function == nullptr) {
    std::cerr << "Attempting to call function " << functionName
              << " that does not exist\n";
    statementGenerationFailed = true;
    return;
  }

  if (function->arg_size() != functionCall->GetArguments()->size()) {
    std::cerr << "Invalid number of arguments for call to function "
              << functionCall->GetName() << '\n';
    statementGenerationFailed = true;
    return;
  }

  std::vector<llvm::Value *> argumentValues;
  const auto *arguments = functionCall->GetArguments();
  for (const auto &argumentExpression : *arguments) {
    argumentExpression->Visit(*this);
    if (lastResult == nullptr) {
      std::cerr << "Invalid argument for function call";
      statementGenerationFailed = true;
      return;
    }
    argumentValues.push_back(lastResult);
  }

  builder->CreateCall(function, argumentValues, "calltmp");
}

void CodegenVisitor::VisitIfStatement(IfStatementAST *ifStatement) {
  ifStatement->GetCondition()->Visit(*this);
  llvm::Value *conditionValue = lastResult;
  if (conditionValue == nullptr) {
    statementGenerationFailed = true;
    return;
  }

  // TODO evaluate condition

  llvm::Function *function = builder->GetInsertBlock()->getParent();
  llvm::BasicBlock *thenBlock =
      llvm::BasicBlock::Create(*context, "then", function);
  llvm::BasicBlock *elseBlock =
      llvm::BasicBlock::Create(*context, "else", function);
  llvm::BasicBlock *mergeBlock =
      llvm::BasicBlock::Create(*context, "merge", function);

  builder->CreateCondBr(conditionValue, thenBlock, elseBlock);

  builder->SetInsertPoint(thenBlock);
  ifStatement->GetThenBranch()->Visit(*this);
  if (statementGenerationFailed) {
    return;
  }
  builder->CreateBr(mergeBlock);

  builder->SetInsertPoint(elseBlock);
  ifStatement->GetElseBranch()->Visit(*this);
  if (statementGenerationFailed) {
    return;
  }
  builder->CreateBr(mergeBlock);

  builder->SetInsertPoint(mergeBlock);
}

void CodegenVisitor::VisitStatementBlock(StatementBlockAST *statmentBlock) {
  std::cerr << "Creating statement block\n";
  // push a new scope for the block
  scopeStack->PushScope();
  std::cerr << "Statement Block scope created\n";
  // then generate the statements for the block
  auto *body = statmentBlock->GetBody();
  for (auto &statement : *body) {
    statement->Visit(*this);
    if (statementGenerationFailed) {
      return;
    }
  }

  std::cerr << "Removing statement scope\n";
  // remove the block scope from the stack
  scopeStack->PopScope();
  std::cerr << "Statement scope removed. Generation completed\n";
}

void CodegenVisitor::VisitAssignmentStatement(
    AssignmentStatementAST *assignmentStatement) {
  // get the function that the statement will live in
  llvm::Function *function = builder->GetInsertBlock()->getParent();

  // Register the variable and emit its initializer
  // first pull the name and initalizer from the AST node
  const std::string variableName = assignmentStatement->GetName();
  ExpressionAST *initalizer = assignmentStatement->GetInitalizer();

  // then emit the initalizer before adding the variable to scope to prevent
  // the initalizer from referencing the variable itself
  llvm::Value *initialValue;
  // if the initalizer exists, grab the inital value of the variable
  if (initalizer != nullptr) {
    initalizer->Visit(*this);
    if (lastResult == nullptr) {
      std::cerr << "Invalid Initalizer for variable " << variableName << "\n";
      statementGenerationFailed = true;
      return;
    }
    initialValue = lastResult;
  }
  // else set it to the default, which for now is 0
  else {
    // TODO update this to get correct type if/when types are added
    // assume int for now, will need to change when we add types
    initialValue =
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0, true);
  }

  // create the alloca point for the variable
  llvm::AllocaInst *alloca = CreateEntryBlockAlloca(function, variableName);
  // emit a store for the inital value into the alloca
  builder->CreateStore(initialValue, alloca);

  // finally add the variable to scope
  scopeStack->AddVariable(variableName, alloca);
  std::cerr << "Variable " << variableName << "Added to scope stack\n";
}

void CodegenVisitor::VisitIntExpression(IntExpressionAST *intExpression) {
  lastResult = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context),
                                      intExpression->GetValue(), true);
}

void CodegenVisitor::VisitDoubleExpression(
    DoubleExpressionAST *doubleExpression) {
  lastResult = llvm::ConstantFP::get(
      *context, llvm::APFloat(doubleExpression->GetValue()));
}

void CodegenVisitor::VisitVariableExpression(
    VariableExpressionAST *variableExpression) {
  llvm::AllocaInst *alloca =
      scopeStack->GetVariableValue(variableExpression->GetName());
  if (alloca == nullptr) {
    std::cerr << "Variable " << *variableExpression->GetName()
              << " Does not exist\n";
    lastResult = nullptr;
    return;
  }
  lastResult = builder->CreateLoad(alloca->getAllocatedType(), alloca,
                                   *variableExpression->GetName());
}

void CodegenVisitor::VisitBinaryExpression(
    BinaryExpressionAST *binaryExpression) {
  binaryExpression->GetLHS()->Visit(*this);
  if (lastResult == nullptr) {
    std::cerr << "Invalid left hand side for binary expression\n";
    return;
  }
  llvm::Value *leftHandSide = lastResult;

  binaryExpression->GetRHS()->Visit(*this);
  if (lastResult == nullptr) {
    std::cerr << "Invalid right hand side for binary expression";
    return;
  }
  llvm::Value *rightHandSide = lastResult;

  std::string binaryOperator = *binaryExpression->GetBinaryOperator();
  if (binaryOperator == "⊕") {
    lastResult = builder->CreateNSWAdd(leftHandSide, rightHandSide, "addtmp");
    return;
  }
  if (binaryOperator == "⊖") {
    lastResult = builder->CreateNSWSub(leftHandSide, rightHandSide, "subtmp");
    return;
  }
  if (binaryOperator == "×") {
    lastResult = builder->CreateNSWMul(leftHandSide, rightHandSide, "multmp");
    return;
  }
  if (binaryOperator == "÷") {
    lastResult =
        builder->CreateExactSDiv(leftHandSide, rightHandSide, "divtmp");
    return;
  }

  if (binaryOperator == "≺") {
    lastResult =
        builder->CreateICmpSLT(leftHandSide, rightHandSide, "cmplttmp");
    return;
  }
  if (binaryOperator == "≻") {
    lastResult =
        builder->CreateICmpSGT(leftHandSide, rightHandSide, "cmpgttmp");
    return;
  }
  std::cerr << "Invalid binary operator " << binaryOperator;
  lastResult = nullptr;
}

bool CodegenVisitor::TryGetBuiltInFunctionName(std::string *babelName,
                                               llvm::Type *type,
                                               std::string &outputName) {
  auto builtinNameData = builtinFunctionMap.find(*babelName);
  if (builtinNameData == builtinFunctionMap.end()) {
    return false;
  }

  std::string functionName = builtinNameData->second;

  if (functionName != "print") {
    outputName = functionName;
    return true;
  }

  if (type == llvm::Type::getInt64Ty(*context)) {
    outputName = functionName + "_int";
    return true;
  }

  return false;
}

llvm::Function *
CodegenVisitor::FindOrDeclareBuiltinFunction(std::string *functionName) {

  // if the function has already been created, return it
  llvm::Function *function = module->getFunction(*functionName);
  if (function != nullptr) {
    return function;
  }

  // else emit it
  if (*functionName == "print_int") {
    llvm::FunctionType *printfType = llvm::FunctionType::get(
        builder->getInt64Ty(), {llvm::IntegerType::getInt64Ty(*context)},
        false);
    return llvm::Function::Create(printfType, llvm::Function::ExternalLinkage,
                                  "print_int", module);
  }

  return nullptr;
}

llvm::Function *CodegenVisitor::FindFunction(std::string *name) {
  // check if the function has been declared
  llvm::Function *function = module->getFunction(*name);
  if (function != nullptr) {
    return function;
  }

  return FindOrDeclareBuiltinFunction(name);
}

} // namespace Babel