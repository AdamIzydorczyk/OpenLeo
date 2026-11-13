
void __regparm3 FUN_0002fb30(int param_1,char param_2)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  uint auStack_24 [3];
  undefined1 local_18;
  int local_14;
  char local_10;
  
  local_10 = param_2;
  iVar3 = FUN_0005c320();
  if (iVar3 == 0) {
    iVar3 = 0;
  }
  else {
    iVar1 = *(int *)(param_1 + 0xee);
    iVar3 = FUN_00054030();
    uVar2 = *(undefined4 *)(iVar1 + 0xee);
    *(undefined ***)(iVar3 + 8) = &PTR_LAB_00096874;
    *(undefined ***)(iVar3 + 0xc) = &PTR_LAB_0009686c;
    *(undefined4 *)(iVar3 + 0xee) = uVar2;
  }
  *(undefined4 *)(iVar3 + 0xb1) = *(undefined4 *)(param_1 + 0xb1);
  local_14 = *(int *)(iVar3 + 8);
  (**(code **)(local_14 + 4))();
  *(int *)(iVar3 + 0xee) = param_1;
  local_18 = 0;
  FUN_000749a0();
  auStack_24[(uint)(int)local_10 >> 5] =
       auStack_24[(uint)(int)local_10 >> 5] | 1 << (local_10 % ' ' & 0x1fU);
  FUN_00049580();
  (**(code **)(*(int *)(param_1 + 8) + 0x80))();
  return;
}

