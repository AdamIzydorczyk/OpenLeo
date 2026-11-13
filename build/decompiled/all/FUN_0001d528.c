
undefined8 __regparm2
FUN_0001d528(undefined4 param_1,undefined4 param_2,undefined4 *param_3,undefined4 *param_4,
            int param_5,int param_6)

{
  int iVar1;
  
  do {
    for (iVar1 = 0x14; iVar1 != 0; iVar1 = iVar1 + -1) {
      *param_3 = *param_4;
      param_4 = param_4 + 1;
      param_3 = param_3 + 1;
    }
    param_3 = param_3 + 0x50;
    param_4 = (undefined4 *)((int)param_4 + param_5 + -0x50);
    param_6 = param_6 + -1;
  } while (param_6 != 0);
  return CONCAT44(param_2,param_1);
}

