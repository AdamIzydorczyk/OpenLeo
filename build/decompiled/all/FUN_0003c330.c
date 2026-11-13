
undefined4 * __regparm3 FUN_0003c330(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 *extraout_ECX;
  int unaff_EBX;
  undefined4 *puVar2;
  undefined4 *puVar3;
  byte bVar4;
  undefined4 local_24 [3];
  undefined1 local_18;
  undefined4 *local_14;
  undefined4 *local_10;
  
  bVar4 = 0;
  local_10 = local_24;
  local_18 = 0;
  local_14 = param_2;
  FUN_000749a0(0,0,0,0,&PTR_FUN_00098274);
  (*(code *)**(undefined4 **)(**(int **)(extraout_ECX[0xf] + 8) + 0x33))
            (*extraout_ECX,extraout_ECX[1]);
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x6c))();
  iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
  if (iVar1 != 0) {
    FUN_00052b10();
  }
  *(undefined1 *)(local_14 + 3) = local_18;
  puVar2 = local_24;
  puVar3 = local_14;
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
  return local_14;
}

