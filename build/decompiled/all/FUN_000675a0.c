
void FUN_000675a0(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  int extraout_ECX_04;
  int extraout_ECX_05;
  int extraout_ECX_06;
  int extraout_ECX_07;
  int extraout_ECX_08;
  int local_1c;
  int local_18;
  int *local_14;
  
  iVar1 = FUN_0005c320();
  iVar2 = 0;
  iVar3 = extraout_ECX;
  if (iVar1 != 0) {
    iVar2 = FUN_0003fb30();
    *(undefined ***)(iVar2 + 0x2e) = &PTR_LAB_000a42a4;
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
    *(undefined ***)(iVar2 + 0x2e) = &PTR_LAB_000a4288;
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
    iVar2 = FUN_0003fb30();
    *(undefined ***)(iVar2 + 0x2e) = &PTR_LAB_000a426c;
    iVar3 = extraout_ECX_04;
  }
  if (*(int *)(iVar3 + 0x10e) == *(int *)(iVar3 + 0x112)) {
    FUN_00021930();
  }
  local_18 = *(int *)(iVar3 + 0x10e) * 4;
  *(int *)(iVar3 + 0x10e) = *(int *)(iVar3 + 0x10e) + 1;
  *(int *)(*(int *)(iVar3 + 0x106) + local_18) = iVar2;
  iVar1 = FUN_0005c320();
  iVar2 = 0;
  iVar3 = extraout_ECX_05;
  if (iVar1 != 0) {
    iVar2 = FUN_0003fb30();
    *(undefined ***)(iVar2 + 0x2e) = &PTR_LAB_000a4250;
    iVar3 = extraout_ECX_06;
  }
  if (*(int *)(iVar3 + 0x10e) == *(int *)(iVar3 + 0x112)) {
    FUN_00021930();
  }
  iVar1 = *(int *)(iVar3 + 0x10e);
  *(int *)(iVar3 + 0x10e) = iVar1 + 1;
  *(int *)(*(int *)(iVar3 + 0x106) + iVar1 * 4) = iVar2;
  iVar3 = FUN_0005c320();
  local_1c = 0;
  iVar2 = extraout_ECX_07;
  if (iVar3 != 0) {
    local_1c = FUN_0003fb30();
    *(undefined ***)(local_1c + 0x2e) = &PTR_LAB_000a4234;
    iVar2 = extraout_ECX_08;
  }
  local_14 = &local_1c;
  if (*(int *)(iVar2 + 0x10e) == *(int *)(iVar2 + 0x112)) {
    FUN_00021930();
  }
  iVar3 = *(int *)(iVar2 + 0x10e);
  *(int *)(iVar2 + 0x10e) = iVar3 + 1;
  *(int *)(*(int *)(iVar2 + 0x106) + iVar3 * 4) = *local_14;
  FUN_0003ffb0();
  return;
}

