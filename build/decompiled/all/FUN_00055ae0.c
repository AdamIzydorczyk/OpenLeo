
int __regparm3 FUN_00055ae0(undefined4 *param_1)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined4 unaff_EBX;
  
  *param_1 = 1;
  param_1[2] = unaff_EBX;
  param_1[4] = 0;
  param_1[5] = &PTR_FUN_00096200;
  FUN_00057990();
  *(undefined ***)(extraout_ECX + 4) = &PTR_FUN_000961fc;
  *(undefined ***)(extraout_ECX + 8) = &PTR_LAB_000a2744;
  iVar1 = FUN_00010254();
  *(int *)(extraout_ECX_00 + -0xc) = iVar1;
  *(undefined4 *)(extraout_ECX_00 + -4) = *(undefined4 *)(iVar1 + 5);
  return extraout_ECX_00 + -0x10;
}

