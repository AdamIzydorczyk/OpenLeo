
uint __regparm3 FUN_0007b9b0(uint param_1,int *param_2)

{
  int iVar1;
  uint uVar2;
  uint extraout_ECX;
  int extraout_EDX;
  int *extraout_EDX_00;
  undefined8 uVar3;
  
  if ((*(byte *)(param_2 + 3) & 2) == 0) {
    FUN_00078c12();
    *(byte *)(extraout_EDX + 0xc) = *(byte *)(extraout_EDX + 0xc) | 0x20;
LAB_0007b9cb:
    param_1 = 0xffffffff;
  }
  else {
    if (param_2[2] == 0) {
      FUN_0007a448();
      param_2 = extraout_EDX_00;
    }
    uVar2 = 0x400;
    if ((param_1 == 10) && (uVar2 = 0x600, (*(byte *)(param_2 + 3) & 0x40) == 0)) {
      *(byte *)((int)param_2 + 0xd) = *(byte *)((int)param_2 + 0xd) | 0x10;
      *(undefined1 *)*param_2 = 0xd;
      iVar1 = param_2[1];
      *param_2 = *param_2 + 1;
      param_2[1] = iVar1 + 1;
      if (iVar1 + 1 == param_2[5]) {
        uVar3 = FUN_0007a4e4();
        param_2 = (int *)((ulonglong)uVar3 >> 0x20);
        uVar2 = extraout_ECX;
        if ((int)uVar3 != 0) goto LAB_0007b9cb;
      }
    }
    *(byte *)((int)param_2 + 0xd) = *(byte *)((int)param_2 + 0xd) | 0x10;
    *(char *)*param_2 = (char)param_1;
    iVar1 = param_2[1];
    *param_2 = *param_2 + 1;
    param_2[1] = iVar1 + 1;
    if (((uVar2 & param_2[3]) != 0) || (iVar1 + 1 == param_2[5])) {
      iVar1 = FUN_0007a4e4();
      if (iVar1 != 0) goto LAB_0007b9cb;
    }
    param_1 = param_1 & 0xff;
  }
  return param_1;
}

