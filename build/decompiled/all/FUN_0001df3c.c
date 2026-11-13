
undefined8 __regparm2
FUN_0001df3c(undefined4 param_1,undefined4 param_2,byte *param_3,byte *param_4,int param_5,
            int param_6,int param_7)

{
  byte *pbVar1;
  int iVar2;
  
  do {
    iVar2 = param_5;
    do {
      pbVar1 = param_3 + 1;
      if (*param_3 < *param_4) {
        *param_4 = *param_3;
      }
      param_4 = param_4 + 1;
      iVar2 = iVar2 + -1;
      param_3 = pbVar1;
    } while (iVar2 != 0);
    param_3 = pbVar1 + param_6;
    param_4 = param_4 + (0x140 - param_5);
    param_7 = param_7 + -1;
  } while (param_7 != 0);
  return CONCAT44(param_2,param_1);
}

