
void __regparm3 FUN_0001ece0(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  
  iVar3 = FUN_0005c320();
  if (iVar3 == 0) {
    iVar3 = 0;
  }
  else {
    iVar1 = *(int *)(param_1 + 0x120);
    iVar3 = FUN_00054030();
    uVar2 = *(undefined4 *)(iVar1 + 0xee);
    *(undefined ***)(iVar3 + 8) = &PTR_LAB_00096874;
    *(undefined ***)(iVar3 + 0xc) = &PTR_LAB_0009686c;
    *(undefined4 *)(iVar3 + 0xee) = uVar2;
  }
  *(undefined4 *)(iVar3 + 0xb1) = *(undefined4 *)(param_1 + 0xb1);
  FUN_00052b10();
  (**(code **)(*(int *)(iVar3 + 8) + 4))();
  *(int *)(iVar3 + 0xee) = param_1;
  (**(code **)(*(int *)(param_1 + 8) + 0x80))();
  return;
}

