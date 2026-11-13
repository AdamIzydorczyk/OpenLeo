
undefined4 * __regparm3 FUN_000553e0(int param_1,undefined4 *param_2)

{
  int extraout_ECX;
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  byte bVar4;
  undefined4 local_28 [3];
  undefined1 local_1c;
  undefined4 *local_18;
  
  bVar4 = 0;
  local_1c = 0;
  local_18 = param_2;
  FUN_000749a0();
  if (0 < *(int *)(param_1 + 0x3c)) {
    iVar1 = 0;
    do {
      (**(code **)(*(int *)(*(int *)(iVar1 + *(int *)(param_1 + 0x40)) + 8) + 0x78))();
      FUN_00049580();
      iVar1 = iVar1 + 4;
    } while (extraout_ECX < *(int *)(param_1 + 0x3c));
  }
  *(undefined1 *)(local_18 + 3) = local_1c;
  puVar2 = local_28;
  puVar3 = local_18;
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
  return local_18;
}

