
int * __regparm3 FUN_0007aab0(int *param_1,uint param_2)

{
  undefined1 *puVar1;
  int iVar2;
  uint extraout_ECX;
  uint uVar3;
  
  param_2 = param_2 & 0xff;
  iVar2 = 0;
  if (*(int *)((int)param_1 + *(int *)(*param_1 + 4) + 0x10) == 0) {
    if ((*(int *)((int)param_1 + *(int *)(*param_1 + 4) + 8) == 0) &&
       ((*(uint *)((int)param_1 + *(int *)(*param_1 + 4) + 0xc) & 0x4000) == 0)) {
      iVar2 = 1;
    }
    else {
      iVar2 = FUN_0007a648();
      param_2 = extraout_ECX;
    }
  }
  if (iVar2 != 0) {
    iVar2 = *(int *)((int)param_1 + *(int *)(*param_1 + 4) + 4);
    uVar3 = param_2 & 0xff;
    puVar1 = *(undefined1 **)(iVar2 + 0x20);
    if (puVar1 < *(undefined1 **)(iVar2 + 0x1c)) {
      *(undefined1 **)(iVar2 + 0x20) = puVar1 + 1;
      *puVar1 = (char)param_2;
    }
    else {
      uVar3 = (**(code **)(*(int *)(iVar2 + 0x28) + 0xc))();
    }
    if (uVar3 == 0xffffffff) {
      FUN_0007a9c0();
    }
    if ((*(uint *)((int)param_1 + *(int *)(*param_1 + 4) + 0xc) & 0x2000) != 0) {
      FUN_0007a9fc();
    }
  }
  return param_1;
}

