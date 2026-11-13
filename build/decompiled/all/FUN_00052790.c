
undefined4 * __regparm3 FUN_00052790(int param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  byte bVar4;
  undefined4 local_24 [3];
  undefined1 local_18;
  undefined4 *local_14;
  
  bVar4 = 0;
  local_18 = 0;
  local_14 = param_2;
  FUN_000749a0();
  if ((*(byte *)(param_1 + 0x13) & 2) != 0) {
    FUN_0004bd30();
    FUN_00049580();
  }
  if ((*(byte *)(param_1 + 0x12) & 0x40) != 0) {
    FUN_0004b890();
    FUN_00049580();
  }
  if ((*(byte *)(param_1 + 0x12) & 0x80) != 0) {
    FUN_0004b870();
    FUN_00049580();
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

