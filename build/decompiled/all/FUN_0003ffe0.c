
undefined4 __regparm3 FUN_0003ffe0(int param_1)

{
  int iVar1;
  int extraout_ECX;
  int iVar2;
  
  iVar1 = 0;
  iVar2 = 0;
  while( true ) {
    if (*(int *)(param_1 + 0x10e) <= iVar1) {
      return 0;
    }
    iVar1 = FUN_00076d20();
    if (iVar1 == 0) break;
    iVar2 = iVar2 + 4;
    iVar1 = extraout_ECX + 1;
  }
  return *(undefined4 *)(iVar2 + *(int *)(param_1 + 0x106));
}

