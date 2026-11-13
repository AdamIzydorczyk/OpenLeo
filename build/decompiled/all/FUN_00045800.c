
undefined4 * __regparm3 FUN_00045800(int *param_1,undefined4 *param_2)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 *puVar3;
  byte bVar4;
  undefined4 local_24 [3];
  undefined1 local_18;
  undefined4 *local_14;
  
  bVar4 = 0;
  local_14 = param_2;
  if (*param_1 == 0) {
    local_18 = 0;
    FUN_000749a0();
    puVar1 = local_24;
  }
  else {
    puVar1 = (undefined4 *)(**(code **)(*(int *)(*param_1 + 4) + 8))();
  }
  *(undefined1 *)(local_14 + 3) = *(undefined1 *)(puVar1 + 3);
  puVar3 = local_14;
  for (iVar2 = 3; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *puVar1;
    puVar1 = puVar1 + (uint)bVar4 * -2 + 1;
    puVar3 = puVar3 + (uint)bVar4 * -2 + 1;
  }
  for (iVar2 = 0; iVar2 != 0; iVar2 = iVar2 + -1) {
    *(undefined1 *)puVar3 = *(undefined1 *)puVar1;
    puVar1 = (undefined4 *)((int)puVar1 + (uint)bVar4 * -2 + 1);
    puVar3 = (undefined4 *)((int)puVar3 + (uint)bVar4 * -2 + 1);
  }
  return local_14;
}

