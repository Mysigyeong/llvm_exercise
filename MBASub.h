//========================================================================
// FILE:
//    MBASub.h
//
// DESCRIPTION:
//    Declares the MBASub pass for the new and the legacy pass managers.
//
// License: MIT
//========================================================================
#ifndef LLVM_TUTOR_MBA_SUB_H
#define LLVM_TUTOR_MBA_SUB_H

#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"

// PassInfoMixIn is a CRTP mix-in to automatically provide informational APIs
// needed for passes. Currently it provides only the 'name' method.
struct MBASub : public llvm::PassInfoMixin<MBASub> {
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &);
  bool runOnBasicBlock(llvm::BasicBlock &B);
};

#endif