
undefined4 __regparm3 FUN_00059550(int param_1,undefined4 *param_2)

{
  bool bVar1;
  int iVar2;
  undefined4 uVar3;
  
  if (((char *)*param_2 == (char *)0x0) || (*(char *)*param_2 == '\0')) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  if (bVar1) {
    uVar3 = *(undefined4 *)(param_1 + 4);
  }
  else {
    *(undefined4 *)(param_1 + 0x24) = 0;
    uVar3 = *(undefined4 *)(param_1 + 0x1c);
    while (*(undefined4 *)(param_1 + 0x20) = uVar3, *(int *)(param_1 + 0x20) != 0) {
      FUN_00010598();
      iVar2 = FUN_00076d20();
      if (iVar2 == 0) {
        uVar3 = FUN_00059430();
        return uVar3;
      }
      *(int *)(param_1 + 0x24) = *(int *)(param_1 + 0x20);
      uVar3 = *(undefined4 *)(*(int *)(param_1 + 0x20) + 4);
    }
    DAT_000cf0f8 = DAT_000cf0f8 + 1;
    FUN_000588d0();
    iVar2 = FUN_00059550();
    FUN_00057a00();
    if (iVar2 != 0) {
      FUN_00058d10();
      uVar3 = FUN_00059500();
      return uVar3;
    }
    uVar3 = 0;
  }
  return uVar3;
}

