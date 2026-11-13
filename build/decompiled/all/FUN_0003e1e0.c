
void FUN_0003e1e0(void)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  
  iVar1 = FUN_00053ff0();
  *(undefined ***)(iVar1 + 0xfe) = &PTR_FUN_00098274;
  *(undefined4 *)(iVar1 + 0xf2) = 0;
  *(undefined4 *)(iVar1 + 0xf6) = 0;
  *(undefined4 *)(iVar1 + 0xfa) = 0;
  *(undefined4 *)(iVar1 + 0xee) = 0;
  *(undefined ***)(iVar1 + 0x10a) = &PTR_LAB_00098278;
  (**(code **)(*(int *)(iVar1 + 0x10a) + 4))();
  *(undefined ***)(extraout_ECX + 8) = &PTR_LAB_00098278;
  (**(code **)(*(int *)(extraout_ECX + 8) + 4))();
  *(undefined4 *)(extraout_ECX_00 + 0xc) = 0;
  *(undefined4 *)(extraout_ECX_00 + 0x10) = 0;
  *(undefined4 *)(extraout_ECX_00 + 0x14) = 0;
  *(undefined ***)(extraout_ECX_00 + -0x106) = &PTR_LAB_000a080c;
  *(undefined ***)(extraout_ECX_00 + -0x102) = &PTR_LAB_000a0804;
  return;
}

