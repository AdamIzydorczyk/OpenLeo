
void FUN_0006cd60(void)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar3;
  
  iVar1 = FUN_0005c320();
  iVar2 = 0;
  iVar3 = extraout_ECX;
  if (iVar1 != 0) {
    iVar2 = FUN_0003fb30();
    *(undefined ***)(iVar2 + 0x2e) = &PTR_LAB_000a5118;
    iVar3 = extraout_ECX_00;
  }
  if (*(int *)(iVar3 + 0x10e) == *(int *)(iVar3 + 0x112)) {
    FUN_00021930();
  }
  iVar1 = *(int *)(iVar3 + 0x10e);
  *(int *)(iVar3 + 0x10e) = iVar1 + 1;
  *(int *)(*(int *)(iVar3 + 0x106) + iVar1 * 4) = iVar2;
  FUN_0003ffb0();
  return;
}

