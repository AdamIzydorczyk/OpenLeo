
int FUN_00060a30(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 extraout_ECX;
  
  iVar1 = FUN_00060930();
  *(undefined4 *)(iVar1 + 0xa4) = 0;
  *(undefined ***)(iVar1 + 0x9c) = &PTR_LAB_000a2d48;
  *(undefined4 *)(iVar1 + 0xa8) = extraout_ECX;
  iVar2 = FUN_0005c320();
  uVar3 = 0;
  if (iVar2 != 0) {
    uVar3 = FUN_00043e70();
  }
  *(undefined4 *)(iVar1 + 0xa4) = uVar3;
  return iVar1;
}

