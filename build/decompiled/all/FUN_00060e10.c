
int FUN_00060e10(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 in_ECX;
  int extraout_ECX;
  int extraout_ECX_00;
  
  iVar3 = FUN_00060930();
  *(undefined ***)(iVar3 + 0x9c) = &PTR_FUN_000a2d24;
  iVar1 = *(int *)(iVar3 + 0x90);
  iVar2 = *(int *)(iVar1 + 0x10);
  (*(code *)PTR_FUN_0009827c)();
  (**(code **)(iVar2 + 8))();
  iVar1 = *(int *)(iVar1 + 0x10);
  (*(code *)PTR_FUN_0009827c)();
  (**(code **)(iVar1 + 8))();
  iVar1 = *(int *)(extraout_ECX + 0x94);
  iVar2 = *(int *)(iVar1 + 0x10);
  (*(code *)PTR_FUN_0009827c)();
  (**(code **)(iVar2 + 8))();
  iVar1 = *(int *)(iVar1 + 0x10);
  (*(code *)PTR_FUN_0009827c)();
  (**(code **)(iVar1 + 8))();
  *(undefined4 *)(*(int *)(extraout_ECX_00 + 0x84) + 0x30) = in_ECX;
  *(undefined4 *)(*(int *)(extraout_ECX_00 + 0x88) + 0x30) = param_1;
  if (*(int *)(*(int *)(extraout_ECX_00 + 0x88) + 0x30) == 0) {
    *(undefined4 *)(*(int *)(extraout_ECX_00 + 0x8c) + 0x2c) = 2;
  }
  else {
    *(undefined4 *)(*(int *)(extraout_ECX_00 + 0x8c) + 0x2c) = 1;
  }
  return iVar3;
}

