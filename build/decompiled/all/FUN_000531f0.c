
undefined8 __regparm3 FUN_000531f0(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  int extraout_ECX;
  
  FUN_00052860(param_2,param_3);
  iVar2 = FUN_00021e30();
  *(undefined4 *)(iVar2 + 0x14) = 0;
  *(undefined4 *)(iVar2 + 0x1c) = 0;
  uVar1 = DAT_000cf0e4;
  *(undefined4 *)(iVar2 + 0x20) = 1;
  *(undefined4 *)(iVar2 + 0x18) = uVar1;
  *(undefined1 *)(iVar2 + 0x30) = 0;
  FUN_000749a0();
  *(undefined4 *)(extraout_ECX + 0xd) = 1;
  *(undefined4 *)(extraout_ECX + 0x11) = 1;
  *(undefined4 *)(extraout_ECX + 0x15) = 0;
  *(undefined4 *)(extraout_ECX + 0x19) = 0;
  *(undefined4 *)(extraout_ECX + 0x1d) = 0;
  *(undefined4 *)(extraout_ECX + 0x21) = 0;
  *(undefined ***)(extraout_ECX + 0x25) = &PTR_FUN_00098274;
  *(undefined4 *)(extraout_ECX + 0x2d) = 0;
  *(undefined ***)(extraout_ECX + 0x31) = &PTR_LAB_00098278;
  *(undefined4 *)(extraout_ECX + 0x29) = 0;
  *(undefined4 *)(extraout_ECX + 0x35) = 1;
  *(undefined4 *)(extraout_ECX + 0x39) = 1;
  *(undefined4 *)(extraout_ECX + 0x3d) = 0;
  *(undefined4 *)(extraout_ECX + 0x41) = 0;
  *(undefined ***)(extraout_ECX + -0x58) = &PTR_FUN_000a2498;
  *(undefined ***)(extraout_ECX + -0x54) = &PTR_LAB_000a2490;
  return CONCAT44(param_2,extraout_ECX + -0x60);
}

