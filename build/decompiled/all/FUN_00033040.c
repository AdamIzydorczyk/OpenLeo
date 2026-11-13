
void __regparm3 FUN_00033040(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = FUN_0005c320();
  if (iVar2 == 0) {
    iVar2 = 0;
  }
  else {
    iVar3 = *(int *)(param_1 + 0xee);
    iVar2 = FUN_00054030();
    *(undefined4 *)(iVar2 + 0xee) = *(undefined4 *)(iVar3 + 0xee);
    uVar1 = *(undefined4 *)(iVar3 + 0xf2);
    *(undefined ***)(iVar2 + 8) = &PTR_LAB_0009d088;
    *(undefined ***)(iVar2 + 0xc) = &PTR_LAB_0009d080;
    *(undefined4 *)(iVar2 + 0xf2) = uVar1;
  }
  *(undefined4 *)(iVar2 + 0xb1) = *(undefined4 *)(param_1 + 0xb1);
  FUN_00052b10();
  iVar3 = (**(code **)(*(int *)(param_1 + 8) + 0x70))();
  if (iVar3 == 1) {
    iVar3 = *(int *)(iVar2 + 8);
  }
  else {
    iVar3 = *(int *)(iVar2 + 8);
  }
  (**(code **)(iVar3 + 4))();
  *(int *)(iVar2 + 0xee) = param_1;
  (**(code **)(*(int *)(param_1 + 8) + 0x80))();
  return;
}

