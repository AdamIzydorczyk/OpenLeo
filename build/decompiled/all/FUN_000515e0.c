
int FUN_000515e0(undefined4 param_1)

{
  int iVar1;
  
  iVar1 = FUN_00045770();
  *(undefined4 *)(iVar1 + 8) = 0;
  *(undefined4 *)(iVar1 + 0xc) = 0;
  *(undefined4 *)(iVar1 + 0x10) = 1;
  *(undefined4 *)(iVar1 + 0x14) = 0;
  *(undefined ***)(iVar1 + 4) = &PTR_LAB_000a2638;
  FUN_00051660(param_1);
  return iVar1;
}

