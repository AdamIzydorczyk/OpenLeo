
undefined8 __regparm2
FUN_000824d8(undefined4 param_1,undefined4 param_2,int param_3,uint param_4,int *param_5)

{
  int iVar1;
  
  iVar1 = 0;
  if (((param_4 & 0x7fffffff) != 0) || (param_3 != 0)) {
    iVar1 = ((int)(param_4 >> 0x10 & 0x7ff0) >> 4) + -0x3fe;
  }
  *param_5 = iVar1;
  return CONCAT44(param_2,param_1);
}

