
void __regparm3 FUN_000503d0(int param_1,undefined4 *param_2)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined4 *unaff_EBX;
  
  *(undefined4 *)(param_1 + 0x11a) = 0;
  *(undefined4 *)(param_1 + 0x11e) = 0;
  *(undefined1 *)(param_1 + 0xb5) = 0;
  DAT_000a94f0 = DAT_000a94f0 + -1;
  *(undefined4 *)(param_1 + 0xad) = 100;
  FUN_00054f90();
  *(undefined4 *)(DAT_000cef40 + 0x3a8) = 0;
  (**(code **)(*(int *)(extraout_ECX + 8) + 4))();
  *(undefined4 *)(extraout_ECX_00 + 0xc2) = 0;
  *(undefined4 *)(extraout_ECX_00 + 0xc6) = 0;
  *(undefined4 *)(extraout_ECX_00 + 0x6d) = 1;
  *param_2 = 1;
  iVar1 = DAT_000a94f0;
  *unaff_EBX = 0;
  if (iVar1 == 0) {
    FUN_00050290();
  }
  else {
    *(undefined4 *)(DAT_000cef40 + 0x3a0) = 0;
  }
  FUN_00056380();
  FUN_00056300();
  FUN_000563a0();
  FUN_0004ab70();
  return;
}

