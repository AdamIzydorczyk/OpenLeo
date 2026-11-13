
void __regparm3 FUN_00024aa0(int param_1)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int iVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  
  puVar2 = (undefined4 *)(**(code **)(*(int *)(*(int *)(DAT_000cef40 + 0x394) + 8) + 0x68))();
  iVar3 = FUN_0005c320(*puVar2,puVar2[1],&PTR_LAB_00098278);
  if (iVar3 == 0) {
    iVar3 = 0;
  }
  else {
    iVar3 = FUN_00054030();
    *(undefined ***)(iVar3 + 8) = &PTR_LAB_000983a4;
    *(undefined ***)(iVar3 + 0xc) = &PTR_LAB_0009839c;
    *(undefined4 *)(iVar3 + 0xee) = *(undefined4 *)(extraout_ECX + 0xee);
    *(undefined4 *)(iVar3 + 0xf2) = *(undefined4 *)(extraout_ECX + 0xf2);
    uVar1 = *(undefined4 *)(extraout_ECX + 0xf6);
    *(undefined ***)(iVar3 + 8) = &PTR_LAB_00098bdc;
    *(undefined ***)(iVar3 + 0xc) = &PTR_LAB_00098bd4;
    *(undefined4 *)(iVar3 + 0xf6) = uVar1;
  }
  (**(code **)(*(int *)(iVar3 + 8) + 4))();
  FUN_00024e80();
  (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0xd8))();
  (**(code **)(*(int *)(param_1 + 8) + 0x80))();
  DAT_000cec24 = DAT_000cec24 + 1;
  return;
}

