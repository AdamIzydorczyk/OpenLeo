
undefined8 __regparm3 FUN_0007a4e4(undefined4 *param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 *extraout_ECX;
  undefined4 *extraout_ECX_00;
  undefined4 *extraout_ECX_01;
  int extraout_EDX;
  int iVar2;
  int iVar3;
  
  iVar2 = 0;
  if ((*(byte *)((int)param_1 + 0xd) & 0x10) == 0) {
    if ((param_1[2] != 0) &&
       (*(byte *)(param_1 + 3) = *(byte *)(param_1 + 3) & 0xef,
       (*(byte *)((int)param_1 + 0xd) & 0x20) == 0)) {
      iVar3 = 0;
      if (param_1[1] != 0) {
        iVar3 = FUN_0007a5bd();
        param_1 = extraout_ECX_01;
      }
      if (iVar3 == -1) {
        *(byte *)(param_1 + 3) = *(byte *)(param_1 + 3) | 0x20;
        iVar2 = -1;
      }
    }
  }
  else {
    *(byte *)((int)param_1 + 0xd) = *(byte *)((int)param_1 + 0xd) & 0xef;
    if (((*(byte *)(param_1 + 3) & 2) != 0) && (param_1[2] != 0)) {
      iVar3 = param_1[1];
      while ((iVar3 != 0 && (iVar2 == 0))) {
        iVar1 = FUN_0007b930();
        param_1 = extraout_ECX;
        if (iVar1 == -1) {
          *(byte *)(extraout_ECX + 3) = *(byte *)(extraout_ECX + 3) | 0x20;
          iVar2 = -1;
        }
        else if (iVar1 == 0) {
          FUN_00078c12();
          iVar2 = -1;
          *(byte *)(extraout_ECX_00 + 3) = *(byte *)(extraout_ECX_00 + 3) | 0x20;
          param_1 = extraout_ECX_00;
          iVar1 = extraout_EDX;
        }
        iVar3 = iVar3 - iVar1;
      }
    }
  }
  param_1[1] = 0;
  *param_1 = param_1[2];
  return CONCAT44(param_2,iVar2);
}

