#include "Babel/CodegenVisitor.h"
#include "Babel/AbstractSyntaxTree.h"
#include <llvm-18/llvm/ADT/APFloat.h>
#include <llvm-18/llvm/ADT/APInt.h>
#include <llvm-18/llvm/IR/BasicBlock.h>
#include <llvm-18/llvm/IR/DerivedTypes.h>
#include <llvm-18/llvm/IR/Function.h>
#include <llvm-18/llvm/IR/IRBuilder.h>
#include <llvm-18/llvm/IR/Instructions.h>
#include <llvm-18/llvm/IR/Module.h>
#include <llvm-18/llvm/IR/Type.h>
#include <llvm-18/llvm/IR/Constants.h>
#include <llvm-18/llvm/IR/LLVMContext.h>
#include <memory>
#include <vector>
namespace Babel
{
  CodegenVisitor::CodegenVisitor(llvm::LLVMContext *context,
                                 llvm::IRBuilder<> *builder, llvm::Module *module)
      : context(context), builder(builder), module(module) {}

  llvm::AllocaInst *
  CodegenVisitor::CreateEntryBlockAlloca(llvm::Function *function,
                                         std::string variableName)
  {
    llvm::IRBuilder<> tempBuilder(&function->getEntryBlock(),
                                  function->getEntryBlock().begin());
    return tempBuilder.CreateAlloca(llvm::Type::getInt64Ty(*context), nullptr,
                                    variableName);
  }

  void CodegenVisitor::VisitPrototype(PrototypeAST *prototype)
  {
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
    for (auto &Arg : function->args())
    {
      Arg.setName(prototypeArgs->at(index++));
    }
  }

  void CodegenVisitor::VisitFunction(FunctionAST *function)
  {
    std::unique_ptr<PrototypeAST> prototype = function->ClaimPrototype();
    if (prototype == nullptr)
    {
      statementGenerationFailed = true;
      return;
    }
    std::string *prototypeName = prototype->GetName();

    // TODO handle extern functions
    // check if the function already exists
    llvm::Function *llvmFunction = module->getFunction(*prototypeName);

    // if the function already exists, don't redefine it
    if (llvmFunction != nullptr)
    {
      std::cerr << "Function already exists";
      statementGenerationFailed = true;
      return;
    }

    // else generate the prototype
    prototype->Visit(*this);
    if (statementGenerationFailed)
    {
      std::cerr << "Function Prototype generation failed";
      return;
    }

    llvmFunction = module->getFunction(*prototypeName);
    if (llvmFunction == nullptr)
    {
      std::cerr << "Function prototype not found";
      statementGenerationFailed = true;
      return;
    }

    // create the basic block for the function
    llvm::BasicBlock *block =
        llvm::BasicBlock::Create(*context, "entry", llvmFunction);

    // add scope for the function variables
    scopeStack->PushScope();
    // create the function variables and add them to scope
    for (auto &Arg : llvmFunction->args())
    {
      llvm::AllocaInst *alloca =
          CreateEntryBlockAlloca(llvmFunction, Arg.getName().str());
      builder->CreateStore(&Arg, alloca);
      scopeStack->AddVariable(Arg.getName().str(), alloca);
    }

    function->GetBody()->Visit(*this);
    if (statementGenerationFailed)
    {
      return;
    }

    // TODO function verification

    // remove the function scope
    scopeStack->PopScope();
  }

  void CodegenVisitor::VisitFunctionCallStatement(FunctionCallStatementAST *functionCall)
  {
    llvm::Function *function = module->getFunction(*functionCall->GetName());
    if (function == nullptr)
    {
      std::cerr << "Attempting to call function " << functionCall->GetName() << " that does not exist";
      statementGenerationFailed = true;
      return;
    }

    if (function->arg_size() != functionCall->GetArguments()->size())
    {
      std::cerr << "Invalid number of arguments for call to function " << functionCall->GetName();
      statementGenerationFailed = true;
      return;
    }

    std::vector<llvm::Value *> argumentValues;
    const auto *arguments = functionCall->GetArguments();
    for (const auto &argumentExpression : *arguments)
    {
      argumentExpression->Visit(*this);
      if (lastResult == nullptr)
      {
        std::cerr << "Invalid argument for function call";
        statementGenerationFailed = true;
        return;
      }
      argumentValues.push_back(lastResult);
    }

    builder->CreateCall(function, argumentValues, "calltmp");
  }

  void CodegenVisitor::VisitIfStatement(IfStatementAST *ifStatement)
  {
    ifStatement->GetCondition()->Visit(*this);
    llvm::Value *conditionValue = lastResult;
    if (conditionValue == nullptr)
    {
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
    if (statementGenerationFailed)
    {
      return;
    }
    builder->CreateBr(mergeBlock);

    builder->SetInsertPoint(elseBlock);
    ifStatement->GetElseBranch()->Visit(*this);
    if (statementGenerationFailed)
    {
      return;
    }
    builder->CreateBr(mergeBlock);

    builder->SetInsertPoint(mergeBlock);
  }

  void CodegenVisitor::VisitStatementBlock(StatementBlockAST *statmentBlock)
  {
    // push a new scope for the block
    scopeStack->PushScope();

    // then generate the statements for the block
    auto *body = statmentBlock->GetBody();
    for (auto &statement : *body)
    {
      statement->Visit(*this);
      if (statementGenerationFailed)
      {
        return;
      }
    }

    // remove the block scope from the stack
    scopeStack->PopScope();
  }

  void CodegenVisitor::VisitAssignmentStatement(
      AssignmentStatementAST *assignmentStatement)
  {
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
    if (initalizer != nullptr)
    {
      initalizer->Visit(*this);
      if (lastResult == nullptr)
      {
        std::cerr << "Invalid Initalizer for variable " << variableName << "\n";
        statementGenerationFailed = true;
        return;
      }
      initialValue = lastResult;
    }
    // else set it to the default, which for now is 0
    else
    {
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
  }

  void CodegenVisitor::VisitIntExpression(IntExpressionAST *intExpression)
  {
    lastResult = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context),
                                        intExpression->GetValue(), true);
  }

  void CodegenVisitor::VisitDoubleExpression(
      DoubleExpressionAST *doubleExpression)
  {
    lastResult = llvm::ConstantFP::get(
        *context, llvm::APFloat(doubleExpression->GetValue()));
  }

  void CodegenVisitor::VisitVariableExpression(
      VariableExpressionAST *variableExpression)
  {
    llvm::AllocaInst *alloca =
        scopeStack->GetVariableValue(variableExpression->GetName());
    lastResult = builder->CreateLoad(alloca->getAllocatedType(), alloca,
                                     *variableExpression->GetName());
  }

  void CodegenVisitor::VisitBinaryExpression(BinaryExpressionAST *binaryExpression)
  {
    binaryExpression->GetLHS()->Visit(*this);
    if (lastResult == nullptr)
    {
      std::cerr << "Invalid left hand side for binary expression";
      return;
    }
    llvm::Value *leftHandSide = lastResult;

    binaryExpression->GetRHS()->Visit(*this);
    if (lastResult == nullptr)
    {
      std::cerr << "Invalid right hand side for binary expression";
      return;
    }
    llvm::Value *rightHandSide = lastResult;

    std::string binaryOperator = *binaryExpression->GetBinaryOperator();
    if (binaryOperator == "⊕")
    {
      builder->CreateNSWAdd(leftHandSide, rightHandSide, "addtmp");
      return;
    }
    if (binaryOperator == "⊖")
    {
      builder->CreateNSWSub(leftHandSide, rightHandSide, "subtmp");
      return;
    }
    if (binaryOperator == "×")
    {
      builder->CreateNSWMul(leftHandSide, rightHandSide, "multmp");
      return;
    }
    if (binaryOperator == "÷")
    {
      builder->CreateExactSDiv(leftHandSide, rightHandSide, "divtmp");
      return;
    }

    if (binaryOperator == "≺")
    {
      return;
    }
    if (binaryOperator == "≻")
    {
      return;
    }
  }
} // namespace Babel