
void FUN_00064cc0(void)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int iVar3;
  
  iVar1 = FUN_0005c320();
  iVar2 = 0;
  iVar3 = extraout_ECX;
  if (iVar1 != 0) {
    iVar2 = FUN_0003fb30();
    *(undefined4 *)(iVar2 + 0x36) = 0;
    *(undefined ***)(iVar2 + 0x3a) = &PTR_LAB_00098278;
    *(undefined4 *)(iVar2 + 0x32) = 0;
    *(undefined ***)(iVar2 + 0x2e) = &PTR_LAB_000a3bfc;
    iVar3 = extraout_ECX_00;
  }
  if (*(int *)(iVar3 + 0x10e) == *(int *)(iVar3 + 0x112)) {
    FUN_00021930();
  }
  iVar1 = *(int *)(iVar3 + 0x10e);
  *(int *)(iVar3 + 0x10e) = iVar1 + 1;
  *(int *)(*(int *)(iVar3 + 0x106) + iVar1 * 4) = iVar2;
  iVar1 = FUN_0005c320();
  iVar2 = 0;
  iVar3 = extraout_ECX_01;
  if (iVar1 != 0) {
    iVar2 = FUN_0003fb30();
    *(undefined ***)(iVar2 + 0x2e) = &PTR_LAB_000a3be0;
    iVar3 = extraout_ECX_02;
  }
  if (*(int *)(iVar3 + 0x10e) == *(int *)(iVar3 + 0x112)) {
    FUN_00021930();
  }
  iVar1 = *(int *)(iVar3 + 0x10e);
  *(int *)(iVar3 + 0x10e) = iVar1 + 1;
  *(int *)(iVar1 * 4 + *(int *)(iVar3 + 0x106)) = iVar2;
  iVar1 = FUN_0005c320();
  iVar2 = 0;
  iVar3 = extraout_ECX_03;
  if (iVar1 != 0) {
    FUN_0003fb30();
    iVar2 = FUN_00076ab0();
    *(undefined ***)(iVar2 + -4) = &PTR_LAB_000a3bc4;
    iVar2 = iVar2 + -0x32;
    iVar3 = extraout_ECX_04;
  }
  if (*(int *)(iVar3 + 0x10e) == *(int *)(iVar3 + 0x112)) {
    FUN_00021930();
  }
  iVar1 = *(int *)(iVar3 + 0x10e);
  *(int *)(iVar3 + 0x10e) = iVar1 + 1;
  *(int *)(iVar1 * 4 + *(int *)(iVar3 + 0x106)) = iVar2;
  FUN_0003ffb0();
  return;
}

