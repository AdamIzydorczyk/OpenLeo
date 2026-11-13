
undefined4 __regparm3 FUN_0004b0f0(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int unaff_EBX;
  
  iVar2 = 0;
  do {
    if (unaff_EBX < *(int *)(param_1 + 0x44)) {
      for (iVar1 = *(int *)(unaff_EBX * 4 + *(int *)(param_1 + 0x58)); iVar1 != 0;
          iVar1 = *(int *)(iVar1 + 0x34)) {
        if (iVar1 == param_2) {
          return 1;
        }
      }
    }
    iVar2 = iVar2 + 1;
    param_1 = param_1 + 4;
  } while (iVar2 < 3);
  return 0;
}

