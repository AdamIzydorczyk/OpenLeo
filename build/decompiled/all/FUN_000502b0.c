
longlong __regparm3 FUN_000502b0(int param_1,uint param_2)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x102) - (*(int *)(param_1 + 0x102) >> 0x1f);
  if (((int)(CONCAT44(iVar1 >> 0x1f,iVar1 >> 1) % 2) == 0) && (*(int *)(param_1 + 0xf2) != 0)) {
    return CONCAT44(param_2,1);
  }
  return (ulonglong)param_2 << 0x20;
}

