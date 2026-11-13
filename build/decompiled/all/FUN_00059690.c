
undefined4 FUN_00059690(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 extraout_ECX;
  
  FUN_000588d0();
  iVar1 = FUN_00059550();
  FUN_00057a00();
  if (iVar1 == 0) {
    uVar2 = 0;
  }
  else {
    FUN_00058980();
    FUN_00058c50();
    FUN_00057a00();
    *(undefined4 *)(iVar1 + 0x1c) = 1;
    *(undefined4 *)(iVar1 + 0xc) = 1;
    uVar2 = extraout_ECX;
  }
  return uVar2;
}

