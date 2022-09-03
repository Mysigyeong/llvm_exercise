#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

using namespace llvm;

struct PrintPtr : public PassInfoMixin<PrintPtr> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
    bool runOnBasicBlock(BasicBlock &BB);
};

bool PrintPtr::runOnBasicBlock(BasicBlock &BB) {
    bool changed = false;

    for (auto inst = BB.begin(), IE = BB.end(); inst != IE; ++inst) {
        if (!inst->mayReadOrWriteMemory())
            continue;
        
        IRBuilder<> Builder(inst);

        Use* operandList = inst->getOperandList();
        unsigned int operandNo = operandList->getOperandNo();
        for (unsigned int i = 0; i < operandNo; i++) {
            Value* operand = operandList->get();
            if (operand->getType()->isPointerTy()) {
                Value* cmp = Builder.CreateICmpEQ(, ConstantInt::get(, 1));
                SplitBlockAndInsertIfThenElse(/*cond*/, inst, /*then*/, /*else*/);
                // BB.getInstList().insert(inst, newInst);
                break;
            }

            operandList = operandList->getNext();
        }
    }
}