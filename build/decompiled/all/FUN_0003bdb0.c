
longlong __regparm3 FUN_0003bdb0(undefined4 param_1,uint param_2)

{
  int iVar1;
  
  iVar1 = FUN_0003bd40();
  if (iVar1 != 0) {
    iVar1 = FUN_0003bf40();
    if (iVar1 != 0) {
      return CONCAT44(param_2,1);
    }
  }
  return (ulonglong)param_2 << 0x20;
}

