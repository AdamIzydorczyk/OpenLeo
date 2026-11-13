
void __regparm3 FUN_00040eb0(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 extraout_EDX;
  undefined4 extraout_EDX_00;
  undefined8 uVar3;
  
  *(undefined4 *)(param_1 + 0xee) = 0;
  uVar3 = FUN_000498f0(param_2,s_damage____d_000915be,param_1 + 0xb1,param_3);
  if ((int)uVar3 == 0) {
    *(undefined4 *)(param_1 + 0xb1) = 0;
  }
  FUN_00049920((int)((ulonglong)uVar3 >> 0x20),s_bounds____d___d_000915cc,param_1 + 0xf2,
               param_1 + 0xf6);
  FUN_00049920(extraout_EDX,s_scope____d_000915de,param_1 + 0xfe);
  uVar3 = FUN_000498f0(extraout_EDX_00,s____d_000915ea,param_1 + 0xfa,param_3);
  if ((int)uVar3 == 0) {
    *(undefined4 *)(param_1 + 0xfa) = 0;
  }
  else {
    uVar1 = *(undefined4 *)(param_1 + 0xfe);
    *(undefined4 *)(param_1 + 0xfe) = *(undefined4 *)(param_1 + 0xfa);
    *(undefined4 *)(param_1 + 0xfa) = uVar1;
  }
  uVar3 = FUN_00049920((int)((ulonglong)uVar3 >> 0x20),s_energy____d_000915f0,param_1 + 0xad);
  if ((int)uVar3 == 0) {
    *(undefined4 *)(param_1 + 0xad) = 100;
  }
  iVar2 = FUN_000498f0((int)((ulonglong)uVar3 >> 0x20),s____d_000915fe,param_1 + 0x102);
  if (iVar2 == 0) {
    *(undefined4 *)(param_1 + 0x102) = 0;
  }
  return;
}

