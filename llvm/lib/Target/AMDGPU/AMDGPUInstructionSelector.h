//===- AMDGPUInstructionSelector --------------------------------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file declares the targeting of the InstructionSelector class for
/// AMDGPU.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AMDGPU_AMDGPUINSTRUCTIONSELECTOR_H
#define LLVM_LIB_TARGET_AMDGPU_AMDGPUINSTRUCTIONSELECTOR_H

#include "AMDGPU.h"
#include "AMDGPUArgumentUsageInfo.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelector.h"
#include "llvm/IR/InstrTypes.h"

namespace {
#define GET_GLOBALISEL_PREDICATE_BITSET
#define AMDGPUSubtarget GCNSubtarget
#include "AMDGPUGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATE_BITSET
#undef AMDGPUSubtarget
}

namespace llvm {

class AMDGPUInstrInfo;
class AMDGPURegisterBankInfo;
class GCNSubtarget;
class MachineInstr;
class MachineIRBuilder;
class MachineOperand;
class MachineRegisterInfo;
class RegisterBank;
class SIInstrInfo;
class SIMachineFunctionInfo;
class SIRegisterInfo;

class AMDGPUInstructionSelector : public InstructionSelector {
private:
  MachineRegisterInfo *MRI;

public:
  AMDGPUInstructionSelector(const GCNSubtarget &STI,
                            const AMDGPURegisterBankInfo &RBI,
                            const AMDGPUTargetMachine &TM);

  bool select(MachineInstr &I) override;
  static const char *getName();

  void setupMF(MachineFunction &MF, GISelKnownBits &KB,
               CodeGenCoverage &CoverageInfo) override;

private:
  struct GEPInfo {
    const MachineInstr &GEP;
    SmallVector<unsigned, 2> SgprParts;
    SmallVector<unsigned, 2> VgprParts;
    int64_t Imm;
    GEPInfo(const MachineInstr &GEP) : GEP(GEP), Imm(0) { }
  };

  bool isInstrUniform(const MachineInstr &MI) const;
  bool isVCC(Register Reg, const MachineRegisterInfo &MRI) const;

  const RegisterBank *getArtifactRegBank(
    Register Reg, const MachineRegisterInfo &MRI,
    const TargetRegisterInfo &TRI) const;

  /// tblgen-erated 'select' implementation.
  bool selectImpl(MachineInstr &I, CodeGenCoverage &CoverageInfo) const;

  MachineOperand getSubOperand64(MachineOperand &MO,
                                 const TargetRegisterClass &SubRC,
                                 unsigned SubIdx) const;

  bool constrainCopyLikeIntrin(MachineInstr &MI, unsigned NewOpc) const;
  bool selectCOPY(MachineInstr &I) const;
  bool selectPHI(MachineInstr &I) const;
  bool selectG_TRUNC(MachineInstr &I) const;
  bool selectG_SZA_EXT(MachineInstr &I) const;
  bool selectG_CONSTANT(MachineInstr &I) const;
  bool selectG_FNEG(MachineInstr &I) const;
  bool selectG_AND_OR_XOR(MachineInstr &I) const;
  bool selectG_ADD_SUB(MachineInstr &I) const;
  bool selectG_UADDO_USUBO_UADDE_USUBE(MachineInstr &I) const;
  bool selectG_EXTRACT(MachineInstr &I) const;
  bool selectG_MERGE_VALUES(MachineInstr &I) const;
  bool selectG_UNMERGE_VALUES(MachineInstr &I) const;
  bool selectG_BUILD_VECTOR_TRUNC(MachineInstr &I) const;
  bool selectG_PTR_ADD(MachineInstr &I) const;
  bool selectG_IMPLICIT_DEF(MachineInstr &I) const;
  bool selectG_INSERT(MachineInstr &I) const;

  bool selectInterpP1F16(MachineInstr &MI) const;
  bool selectG_INTRINSIC(MachineInstr &I) const;

  bool selectEndCfIntrinsic(MachineInstr &MI) const;
  bool selectDSOrderedIntrinsic(MachineInstr &MI, Intrinsic::ID IID) const;
  bool selectDSGWSIntrinsic(MachineInstr &MI, Intrinsic::ID IID) const;
  bool selectDSAppendConsume(MachineInstr &MI, bool IsAppend) const;

  bool selectG_INTRINSIC_W_SIDE_EFFECTS(MachineInstr &I) const;
  int getS_CMPOpcode(CmpInst::Predicate P, unsigned Size) const;
  bool selectG_ICMP(MachineInstr &I) const;
  bool hasVgprParts(ArrayRef<GEPInfo> AddrInfo) const;
  void getAddrModeInfo(const MachineInstr &Load, const MachineRegisterInfo &MRI,
                       SmallVectorImpl<GEPInfo> &AddrInfo) const;
  bool selectSMRD(MachineInstr &I, ArrayRef<GEPInfo> AddrInfo) const;

  void initM0(MachineInstr &I) const;
  bool selectG_LOAD_ATOMICRMW(MachineInstr &I) const;
  bool selectG_AMDGPU_ATOMIC_CMPXCHG(MachineInstr &I) const;
  bool selectG_STORE(MachineInstr &I) const;
  bool selectG_SELECT(MachineInstr &I) const;
  bool selectG_BRCOND(MachineInstr &I) const;
  bool selectG_FRAME_INDEX_GLOBAL_VALUE(MachineInstr &I) const;
  bool selectG_PTR_MASK(MachineInstr &I) const;
  bool selectG_EXTRACT_VECTOR_ELT(MachineInstr &I) const;
  bool selectG_INSERT_VECTOR_ELT(MachineInstr &I) const;

  std::pair<Register, unsigned>
  selectVOP3ModsImpl(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectVCSRC(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectVSRC0(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectVOP3Mods0(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectVOP3OMods(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectVOP3Mods(MachineOperand &Root) const;

  ComplexRendererFns selectVOP3NoMods(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectVOP3Mods_nnan(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectVOP3OpSelMods0(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectVOP3OpSelMods(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectSmrdImm(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectSmrdImm32(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectSmrdSgpr(MachineOperand &Root) const;

  template <bool Signed>
  InstructionSelector::ComplexRendererFns
  selectFlatOffsetImpl(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectFlatOffset(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectFlatOffsetSigned(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectMUBUFScratchOffen(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectMUBUFScratchOffset(MachineOperand &Root) const;

  bool isDSOffsetLegal(Register Base, int64_t Offset,
                       unsigned OffsetBits) const;

  std::pair<Register, unsigned>
  selectDS1Addr1OffsetImpl(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectDS1Addr1Offset(MachineOperand &Root) const;

  std::pair<Register, unsigned>
  selectDS64Bit4ByteAlignedImpl(MachineOperand &Root) const;
  InstructionSelector::ComplexRendererFns
  selectDS64Bit4ByteAligned(MachineOperand &Root) const;

  std::pair<Register, int64_t>
  getPtrBaseWithConstantOffset(Register Root,
                               const MachineRegisterInfo &MRI) const;

  // Parse out a chain of up to two g_ptr_add instructions.
  // g_ptr_add (n0, _)
  // g_ptr_add (n0, (n1 = g_ptr_add n2, n3))
  struct MUBUFAddressData {
    Register N0, N2, N3;
    int64_t Offset = 0;
  };

  bool shouldUseAddr64(MUBUFAddressData AddrData) const;

  void splitIllegalMUBUFOffset(MachineIRBuilder &B,
                               Register &SOffset, int64_t &ImmOffset) const;

  MUBUFAddressData parseMUBUFAddress(Register Src) const;

  bool selectMUBUFAddr64Impl(MachineOperand &Root, Register &VAddr,
                             Register &RSrcReg, Register &SOffset,
                             int64_t &Offset) const;

  bool selectMUBUFOffsetImpl(MachineOperand &Root, Register &RSrcReg,
                             Register &SOffset, int64_t &Offset) const;

  InstructionSelector::ComplexRendererFns
  selectMUBUFAddr64(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectMUBUFOffset(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectMUBUFOffsetAtomic(MachineOperand &Root) const;

  InstructionSelector::ComplexRendererFns
  selectMUBUFAddr64Atomic(MachineOperand &Root) const;

  ComplexRendererFns selectSMRDBufferImm(MachineOperand &Root) const;
  ComplexRendererFns selectSMRDBufferImm32(MachineOperand &Root) const;

  void renderTruncImm32(MachineInstrBuilder &MIB, const MachineInstr &MI,
                        int OpIdx = -1) const;

  void renderTruncTImm(MachineInstrBuilder &MIB, const MachineInstr &MI,
                       int OpIdx) const;

  void renderTruncTImm1(MachineInstrBuilder &MIB, const MachineInstr &MI,
                        int OpIdx) const {
    renderTruncTImm(MIB, MI, OpIdx);
  }

  void renderTruncTImm8(MachineInstrBuilder &MIB, const MachineInstr &MI,
                        int OpIdx) const {
    renderTruncTImm(MIB, MI, OpIdx);
  }

  void renderTruncTImm16(MachineInstrBuilder &MIB, const MachineInstr &MI,
                        int OpIdx) const {
    renderTruncTImm(MIB, MI, OpIdx);
  }

  void renderTruncTImm32(MachineInstrBuilder &MIB, const MachineInstr &MI,
                        int OpIdx) const {
    renderTruncTImm(MIB, MI, OpIdx);
  }

  void renderNegateImm(MachineInstrBuilder &MIB, const MachineInstr &MI,
                       int OpIdx) const;

  void renderBitcastImm(MachineInstrBuilder &MIB, const MachineInstr &MI,
                        int OpIdx) const;

  void renderPopcntImm(MachineInstrBuilder &MIB, const MachineInstr &MI,
                       int OpIdx) const;
  void renderExtractGLC(MachineInstrBuilder &MIB, const MachineInstr &MI,
                        int OpIdx) const;
  void renderExtractSLC(MachineInstrBuilder &MIB, const MachineInstr &MI,
                        int OpIdx) const;
  void renderExtractDLC(MachineInstrBuilder &MIB, const MachineInstr &MI,
                        int OpIdx) const;
  void renderExtractSWZ(MachineInstrBuilder &MIB, const MachineInstr &MI,
                        int OpIdx) const;

  bool isInlineImmediate16(int64_t Imm) const;
  bool isInlineImmediate32(int64_t Imm) const;
  bool isInlineImmediate64(int64_t Imm) const;
  bool isInlineImmediate(const APFloat &Imm) const;

  const SIInstrInfo &TII;
  const SIRegisterInfo &TRI;
  const AMDGPURegisterBankInfo &RBI;
  const AMDGPUTargetMachine &TM;
  const GCNSubtarget &STI;
  bool EnableLateStructurizeCFG;
#define GET_GLOBALISEL_PREDICATES_DECL
#define AMDGPUSubtarget GCNSubtarget
#include "AMDGPUGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATES_DECL
#undef AMDGPUSubtarget

#define GET_GLOBALISEL_TEMPORARIES_DECL
#include "AMDGPUGenGlobalISel.inc"
#undef GET_GLOBALISEL_TEMPORARIES_DECL
};

} // End llvm namespace.
#endif
