; NOTE: Assertions have been autogenerated by utils/update_test_checks.py

; Eliminates store to %R in the entry block.
; RUN: opt < %s -basicaa -dse -enable-dse-memoryssa -debug-counter=dse-memoryssa-skip=0,dse-memoryssa-count=1 -S | FileCheck --check-prefix=SKIP0-COUNT1 %s

; Eliminates store to %P in the entry block.
; RUN: opt < %s -basicaa -dse -enable-dse-memoryssa -debug-counter=dse-memoryssa-skip=1,dse-memoryssa-count=1 -S | FileCheck --check-prefix=SKIP1-COUNT1 %s

; Eliminates both stores in the entry block.
; RUN: opt < %s -basicaa -dse -enable-dse-memoryssa -debug-counter=dse-memoryssa-skip=0,dse-memoryssa-count=2 -S | FileCheck --check-prefix=SKIP0-COUNT2 %s

; Eliminates no stores.
; RUN: opt < %s -basicaa -dse -enable-dse-memoryssa -debug-counter=dse-memoryssa-skip=2,dse-memoryssa-count=1 -S | FileCheck --check-prefix=SKIP2-COUNT1 %s


target datalayout = "e-m:e-p:32:32-i64:64-v128:64:128-a:0:32-n32-S64"


define void @test(i32* noalias %P, i32* noalias %Q, i32* noalias %R) {
; SKIP0-COUNT1-LABEL: @test(
; SKIP0-COUNT1-NEXT:    store i32 1, i32* [[P:%.*]]
; SKIP0-COUNT1-NEXT:    br i1 true, label [[BB1:%.*]], label [[BB2:%.*]]
; SKIP0-COUNT1:       bb1:
; SKIP0-COUNT1-NEXT:    br label [[BB3:%.*]]
; SKIP0-COUNT1:       bb2:
; SKIP0-COUNT1-NEXT:    br label [[BB3]]
; SKIP0-COUNT1:       bb3:
; SKIP0-COUNT1-NEXT:    store i32 0, i32* [[Q:%.*]]
; SKIP0-COUNT1-NEXT:    store i32 0, i32* [[R:%.*]]
; SKIP0-COUNT1-NEXT:    store i32 0, i32* [[P]]
; SKIP0-COUNT1-NEXT:    ret void
;
; SKIP1-COUNT1-LABEL: @test(
; SKIP1-COUNT1-NEXT:    store i32 1, i32* [[R:%.*]]
; SKIP1-COUNT1-NEXT:    br i1 true, label [[BB1:%.*]], label [[BB2:%.*]]
; SKIP1-COUNT1:       bb1:
; SKIP1-COUNT1-NEXT:    br label [[BB3:%.*]]
; SKIP1-COUNT1:       bb2:
; SKIP1-COUNT1-NEXT:    br label [[BB3]]
; SKIP1-COUNT1:       bb3:
; SKIP1-COUNT1-NEXT:    store i32 0, i32* [[Q:%.*]]
; SKIP1-COUNT1-NEXT:    store i32 0, i32* [[R]]
; SKIP1-COUNT1-NEXT:    store i32 0, i32* [[P:%.*]]
; SKIP1-COUNT1-NEXT:    ret void
;
; SKIP0-COUNT2-LABEL: @test(
; SKIP0-COUNT2-NEXT:    br i1 true, label [[BB1:%.*]], label [[BB2:%.*]]
; SKIP0-COUNT2:       bb1:
; SKIP0-COUNT2-NEXT:    br label [[BB3:%.*]]
; SKIP0-COUNT2:       bb2:
; SKIP0-COUNT2-NEXT:    br label [[BB3]]
; SKIP0-COUNT2:       bb3:
; SKIP0-COUNT2-NEXT:    store i32 0, i32* [[Q:%.*]]
; SKIP0-COUNT2-NEXT:    store i32 0, i32* [[R:%.*]]
; SKIP0-COUNT2-NEXT:    store i32 0, i32* [[P:%.*]]
; SKIP0-COUNT2-NEXT:    ret void
;
; SKIP2-COUNT1-LABEL: @test(
; SKIP2-COUNT1-NEXT:    store i32 1, i32* [[P:%.*]]
; SKIP2-COUNT1-NEXT:    store i32 1, i32* [[R:%.*]]
; SKIP2-COUNT1-NEXT:    br i1 true, label [[BB1:%.*]], label [[BB2:%.*]]
; SKIP2-COUNT1:       bb1:
; SKIP2-COUNT1-NEXT:    br label [[BB3:%.*]]
; SKIP2-COUNT1:       bb2:
; SKIP2-COUNT1-NEXT:    br label [[BB3]]
; SKIP2-COUNT1:       bb3:
; SKIP2-COUNT1-NEXT:    store i32 0, i32* [[Q:%.*]]
; SKIP2-COUNT1-NEXT:    store i32 0, i32* [[R]]
; SKIP2-COUNT1-NEXT:    store i32 0, i32* [[P]]
; SKIP2-COUNT1-NEXT:    ret void
;
  store i32 1, i32* %P
  store i32 1, i32* %R
  br i1 true, label %bb1, label %bb2
bb1:
  br label %bb3
bb2:
  br label %bb3
bb3:
  store i32 0, i32* %Q
  store i32 0, i32* %R
  store i32 0, i32* %P
  ret void
}
