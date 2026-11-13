
void __regparm3 FUN_0002e610(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  
  FUN_00050720();
  puVar1 = (undefined4 *)(**(code **)(*(int *)(param_1 + 8) + 0x68))();
  iVar2 = FUN_0002ddb0(*puVar1,puVar1[1],&PTR_LAB_00098278);
  *(int *)(param_1 + 0x1ca) = iVar2;
  if (iVar2 == -1) {
    iVar2 = (**(code **)(*(int *)(param_1 + 8) + 8))();
    if (iVar2 != 0) {
      *(byte *)(extraout_ECX_00 + 2) = *(byte *)(extraout_ECX_00 + 2) | 0x80;
    }
  }
  else {
    iVar2 = (**(code **)(*(int *)(param_1 + 8) + 0x70))();
    if (iVar2 == *(int *)(param_1 + 0x1aa + *(int *)(param_1 + 0x1ca) * 4)) {
      *(byte *)(extraout_ECX + 2) = *(byte *)(extraout_ECX + 2) | 0x40;
    }
  }
  return;
}

