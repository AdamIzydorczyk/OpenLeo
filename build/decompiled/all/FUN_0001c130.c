
longlong __regparm3 FUN_0001c130(int *param_1,uint param_2)

{
  int iVar1;
  
  if (*(int *)param_1[3] == param_1[2]) {
    iVar1 = *param_1;
    *param_1 = iVar1 + 1;
    if (param_1[1] < iVar1 + 1) {
      return CONCAT44(param_2,1);
    }
  }
  else {
    *param_1 = 0;
  }
  return (ulonglong)param_2 << 0x20;
}

