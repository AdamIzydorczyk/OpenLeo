
void __regparm3 FUN_000448c0(undefined4 param_1,undefined4 param_2)

{
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int iVar1;
  
  FUN_00044660(param_2);
  iVar1 = extraout_EDX;
  if (*(int *)(extraout_EDX + 0x50) != 0) {
    if (*(int *)(extraout_EDX + 0x58) == 0) {
      FUN_000447a0();
      iVar1 = extraout_EDX_01;
    }
    else {
      FUN_00044710();
      iVar1 = extraout_EDX_00;
    }
  }
  if (*(int *)(iVar1 + 0x54) != 0) {
    FUN_00044810();
  }
  return;
}

