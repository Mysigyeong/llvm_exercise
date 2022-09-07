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

    // This is very important. When clang optimization option is -O0
    // If I don't specify this function, opt is not executed
    static bool isRequired(void) { return true; }
};


bool PrintPtr::runOnBasicBlock(BasicBlock &BB) {
    bool changed = false;

    for (auto inst = BB.begin(), IE = BB.end(); inst != IE; ++inst) {
        // Can the instruction read or write memory?
        if (!inst->mayReadOrWriteMemory())
            continue;
        
        if (inst->getFunction()->getName().equals("print"))
            continue;

        //errs() << inst->getOpcodeName() << "\n";

        // iterating the operand (Use) of the instruction
        for (auto iter = inst->op_begin(), iter_end = inst->op_end(); iter != iter_end; ++iter) {
            // iter: iterator of Use
            Value* operand = iter->get();

            // Is the type of operand pointer?
            if (operand->getType()->isPointerTy()) {
                IRBuilder<> Builder(&*inst);
                // TODO: Call printf
                Function* printFunc = inst->getModule()->getFunction("print");
                if (printFunc == nullptr) {
                    errs() << "can't find print" << "\n";
                    exit(1);
                }
                
                ArrayRef<Value*> args(operand);
                Instruction* newInst = CallInst::Create(printFunc, args);
                BB.getInstList().insert(inst, newInst);

                // TODO: Make if else

                /*
                // ptr % 2 == 1
                Value* cmp = Builder.CreateICmpEQ(Builder.CreateURem(operand, Builder.getInt64(2)), Builder.getInt64(1));

                Instruction *ThenTerm = nullptr;
                Instruction *ElseTerm = nullptr;
                SplitBlockAndInsertIfThenElse(cmp, inst, &ThenTerm, &ElseTerm);

                Builder.SetInsertPoint(ThenTerm);

                Builder.SetInsertPoint(ElseTerm);
                */
                // BB.getInstList().insert(inst, newInst);
                break;
            }
        }
    }

    return changed;
}

PreservedAnalyses PrintPtr::run(Function &F, FunctionAnalysisManager &) {
    bool Changed = false;
    for (auto &BB : F) {
        Changed |= runOnBasicBlock(BB);
    }
    return (Changed ? PreservedAnalyses::none() : PreservedAnalyses::all());
}


PassPluginLibraryInfo getPrintPtrPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "PrintPtr", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager& FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "print-ptr") {
                        FPM.addPass(PrintPtr());
                        return true;
                    }
                    return false;
                }
            );
        }    
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return getPrintPtrPluginInfo();
}