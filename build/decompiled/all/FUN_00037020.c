
void __regparm3 FUN_00037020(int param_1)

{
  int iVar2;
  int iVar1;
  int iVar3;
  int iVar4;
  
  *(undefined4 *)(param_1 + 0x124) = 0xffffffff;
  *(undefined4 *)(param_1 + 0x120) = 5;
  iVar3 = param_1 + 8;
  *(undefined4 *)(param_1 + 0x128) = 0xffffffff;
  iVar4 = 0;
  do {
    iVar2 = iVar4 * 0x14 + param_1;
    do {
      iVar1 = iVar2 + 4;
      *(undefined4 *)(iVar2 + 0x134) = 0xffffffff;
      *(undefined4 *)(iVar2 + 0x13c) = 0xffffffff;
      iVar2 = iVar1;
    } while (iVar1 != iVar3);
    iVar4 = iVar4 + 1;
    iVar3 = iVar3 + 0x14;
  } while (iVar4 < 0x11);
  return;
}

