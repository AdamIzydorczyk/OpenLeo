
undefined4 * __regparm3 FUN_00055130(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  int iVar2;
  int unaff_EBX;
  undefined4 *puVar3;
  undefined4 *puVar4;
  byte bVar5;
  undefined4 local_34 [3];
  undefined1 local_28;
  undefined4 local_24;
  undefined1 local_18;
  undefined4 *local_14;
  int local_10;
  
  bVar5 = 0;
  iVar2 = *(int *)(unaff_EBX + 0xad);
  local_14 = param_2;
  iVar1 = FUN_000550c0();
  if (iVar1 == 0) {
    local_10 = *(int *)(unaff_EBX + 8);
    (**(code **)(local_10 + 0xd0))();
  }
  if (*(int *)(unaff_EBX + 0xad) < iVar2) {
    local_18 = 0;
    FUN_000749a0();
    puVar3 = &local_24;
    local_24._2_1_ = local_24._2_1_ | 1;
  }
  else {
    local_28 = 0;
    FUN_000749a0();
    puVar3 = local_34;
  }
  *(undefined1 *)(local_14 + 3) = *(undefined1 *)(puVar3 + 3);
  puVar4 = local_14;
  for (iVar2 = 3; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar4 = *puVar3;
    puVar3 = puVar3 + (uint)bVar5 * -2 + 1;
    puVar4 = puVar4 + (uint)bVar5 * -2 + 1;
  }
  for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
    *(undefined1 *)puVar4 = *(undefined1 *)puVar3;
    puVar3 = (undefined4 *)((int)puVar3 + (uint)bVar5 * -2 + 1);
    puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar5 * -2 + 1);
  }
  return local_14;
}

