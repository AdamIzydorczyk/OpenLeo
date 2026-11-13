
void __regparm3 FUN_0003ebc5(int param_1)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  undefined4 uVar4;
  
  FUN_000229c0();
  uVar4 = *(undefined4 *)(param_1 + 0x40);
  iVar1 = *(int *)(DAT_000cef40 + 0x390);
  iVar2 = FUN_00057530(uVar4);
  FUN_00057500(uVar4,*(int *)(param_1 + 4) + (*(int *)(iVar1 + 0x14) - iVar2));
  FUN_000229c0();
  FUN_00042660();
  piVar3 = (int *)FUN_000229c0();
  FUN_00057530((int)*(short *)(*piVar3 + 0x17) - *(int *)(param_1 + 0x40));
  FUN_00057500();
  FUN_000229c0();
  FUN_00042660();
  return;
}

