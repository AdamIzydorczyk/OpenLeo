
undefined4 __regparm3 FUN_00058d80(int param_1,undefined4 *param_2,undefined4 param_3)

{
  bool bVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 uVar4;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined4 unaff_EBX;
  
  if (*(int *)(param_1 + 8) != *(int *)(param_1 + 0x10)) {
    if (((char *)*param_2 == (char *)0x0) || (*(char *)*param_2 == '\0')) {
      bVar1 = true;
    }
    else {
      bVar1 = false;
    }
    if (!bVar1) {
      uVar2 = FUN_00058cd0();
      iVar3 = FUN_00058a70();
      *(undefined1 *)(iVar3 + 0xd) = 0;
      FUN_00076f64();
      *(undefined4 *)(extraout_ECX + 0xe) = unaff_EBX;
      uVar4 = FUN_00059ae0();
      *(undefined4 *)(extraout_ECX_00 + 0x12) = uVar4;
      *(undefined4 *)(extraout_ECX_00 + 0x16) = param_3;
      *(undefined4 *)(param_1 + 0x1c) = 1;
      *(undefined4 *)(param_1 + 0xc) = 1;
      *(int *)(param_1 + 8) = *(int *)(param_1 + 8) + 1;
      return uVar2;
    }
  }
  return 0xffffffff;
}

