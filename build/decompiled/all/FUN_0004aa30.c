
undefined4 __regparm3 FUN_0004aa30(int param_1,int param_2)

{
  int iVar1;
  
  if (param_2 == *(int *)(param_1 + 0x394)) {
    return 1;
  }
  iVar1 = *(int *)(param_1 + 100);
  if (iVar1 == 0) {
    return 0;
  }
  do {
    if (param_2 == iVar1) {
      return 1;
    }
    iVar1 = *(int *)(iVar1 + 0x34);
  } while (iVar1 != 0);
  return 0;
}

