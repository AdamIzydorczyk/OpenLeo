
uint __regparm3 FUN_0007719d(uint param_1,int *param_2)

{
  int iVar1;
  undefined1 *puVar2;
  int *extraout_EDX;
  
  if (param_1 == 0xffffffff) {
    return 0xffffffff;
  }
  if (((*(byte *)((int)param_2 + 0xd) & 0x10) == 0) && ((*(byte *)(param_2 + 3) & 1) != 0)) {
    if (param_2[2] == 0) {
      FUN_0007a448();
      param_2 = extraout_EDX;
    }
    if (param_2[1] == 0) {
      *param_2 = param_2[2] + param_2[5] + -1;
      *(byte *)(param_2 + 3) = *(byte *)(param_2 + 3) | 4;
      puVar2 = (undefined1 *)*param_2;
      param_2[1] = 1;
    }
    else {
      if (*param_2 == param_2[2]) goto LAB_000771b1;
      iVar1 = *param_2;
      param_2[1] = param_2[1] + 1;
      *param_2 = iVar1 + -1;
      if (*(byte *)(iVar1 + -1) != param_1) {
        *(byte *)(param_2 + 3) = *(byte *)(param_2 + 3) | 4;
      }
      puVar2 = (undefined1 *)*param_2;
    }
    *puVar2 = (char)param_1;
    *(byte *)(param_2 + 3) = *(byte *)(param_2 + 3) & 0xef;
    param_1 = param_1 & 0xff;
  }
  else {
LAB_000771b1:
    param_1 = 0xffffffff;
  }
  return param_1;
}

