//==============================================================================
// FILE:
//    MBASub.cc
//
// DESCRIPTION:
//    Obfuscation for integer sub instructions through Mixed Boolean Arithmetic
//    (MBA). This pass performs an instruction substitution based on this
//    equality:
//      a - b == (a + ~b) + 1
//    See formula 2.2 (j) in [1].
//
// USAGE:
//    New pass maanger:
//      $ opt -load-pass-plugin <BUILD_DIR>/lib/libMBASub.so `\`
//        -passes=-"mba-sub" <bitcode-file>
//
//  [1] "Hacker's Delight" by Henry S. Warren, Jr.
//
// License: MIT
//==============================================================================
#include "MBASub.h"

#include "llvm-15/llvm/ADT/Statistic.h"
#include "llvm-15/llvm/IR/IRBuilder.h"
#include "llvm-15/llvm/Passes/PassBuilder.h"
#include "llvm-15/llvm/Passes/PassPlugin.h"
#include "llvm-15/llvm/Transforms/Utils/BasicBlockUtils.h"

#include <random>

using namespace llvm;

#define DEBUG_TYPE "mba-sub"

STATISTIC(SubstCount, "The # of substituted instructions");

//-----------------------------------------------------------------------------
// MBASub Implementaion
//-----------------------------------------------------------------------------
bool MBASub::runOnBasicBlock(BasicBlock &BB) {
    bool Changed = false;

    // Loop over all instructions in the block. Replacing instructions requires
    // iterators, hence a for-range loop wouldn't be suitable.
    for (auto Inst = BB.begin(), IE = BB.end(); Inst != IE; ++Inst) {
        // LLVM API
        // The dyn_cast<> operator is a “checking cast” operation.
        // It checks to see if the operand is of the specified type,
        // and if so, returns a pointer to it (this operator does not work with references).
        // If the operand is not of the correct type, a null pointer is returned.
        // Thus, this works very much like the dynamic_cast<> operator in C++, and should be used in the same circumstances.

        // Skip non-binary (e.g. unary or compare) instruction
        auto* BinOp = dyn_cast<BinaryOperator>(Inst);
        if (!BinOp)
            continue;

        // Skip instructions other than integer sub.
        unsigned Opcode = BinOp->getOpcode();
        if (Opcode != Instruction::Sub || !BinOp->getType()->isIntegerTy())
            continue;
        
        // A uniform API for creating instructions and inserting
        // them into basic blocks.
        IRBuilder<> Builder(BinOp);

        // Create an instruction representing (a + ~b) + 1
        Instruction* NewValue = BinaryOperator::CreateAdd(
            Builder.CreateAdd(BinOp->getOperand(0),
                              Builder.CreateNot(BinOp->getOperand(1))),
            ConstantInt::get(BinOp->getType(), 1));

        // The following is visible only if you pass -debug on the command line
        // and you have an assert build.
        // LLVM_DEBUG(dbgs() << *BinOp << " -> " << *NewValue << "\n");
        errs() << *BinOp << " -> " << *NewValue << "\n";

        // Replace original instruction with the new instruction.
        ReplaceInstWithInst(BB.getInstList(), Inst, NewValue);
        Changed = true;

        // Update the statistics
        ++SubstCount;
    }
    return Changed;
}

PreservedAnalyses MBASub::run(Function &F, FunctionAnalysisManager&) {
    bool Changed = false;

    for (auto &BB : F) {
        Changed |= runOnBasicBlock(BB);
    }
    return (Changed ? PreservedAnalyses::none() : PreservedAnalyses::all());
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
PassPluginLibraryInfo getMBASubPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "mba-sub", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
                PB.registerPipelineParsingCallback(
                    [](StringRef Name, FunctionPassManager& FPM,
                       ArrayRef<PassBuilder::PipelineElement>) {
                        if (Name == "mba-sub") {
                            FPM.addPass(MBASub());
                            return true;
                        }
                        return false;
                });
            }};
}

extern "C" LLVM_ATTRIBUTE_WEAK::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return getMBASubPluginInfo();
}