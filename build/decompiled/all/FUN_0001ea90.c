
undefined4 * __regparm3 FUN_0001ea90(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  int unaff_EBX;
  undefined4 *puVar2;
  undefined4 *puVar3;
  byte bVar4;
  undefined4 local_20;
  byte local_1a;
  undefined1 local_14;
  undefined4 *local_10;
  
  bVar4 = 0;
  local_14 = 0;
  local_10 = param_2;
  FUN_000749a0();
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0xc))();
  if (iVar1 == DAT_000ceba4) {
    iVar1 = FUN_00052eb0();
    if (iVar1 != 0) {
      FUN_00052b10();
      FUN_00052b10();
    }
  }
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0xc))();
  if ((iVar1 == DAT_000cebb0) && (*(int *)(unaff_EBX + 0x5c) == 2)) {
    FUN_000534e0(0,0,0,0,&PTR_FUN_00098274);
    FUN_0004a050();
    FUN_0004a080();
    iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x68))();
    iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
    if (iVar1 != 0) {
      local_1a = local_1a | 0x40;
    }
  }
  *(undefined1 *)(local_10 + 3) = local_14;
  puVar2 = &local_20;
  puVar3 = local_10;
  for (iVar1 = 3; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + (uint)bVar4 * -2 + 1;
    puVar3 = puVar3 + (uint)bVar4 * -2 + 1;
  }
  for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined1 *)puVar3 = *(undefined1 *)puVar2;
    puVar2 = (undefined4 *)((int)puVar2 + (uint)bVar4 * -2 + 1);
    puVar3 = (undefined4 *)((int)puVar3 + (uint)bVar4 * -2 + 1);
  }
  return local_10;
}

