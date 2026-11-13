
longlong __regparm3 FUN_000495b0(int param_1,uint param_2)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  
  uVar3 = 0;
  iVar2 = 0;
  do {
    uVar3 = uVar3 + 1;
    piVar1 = (int *)(param_1 + iVar2);
    iVar2 = iVar2 + 4;
    if (*piVar1 != 0) {
      return (ulonglong)param_2 << 0x20;
    }
  } while (uVar3 < 3);
  return CONCAT44(param_2,1);
}

