
undefined4 * __regparm3 FUN_000381d0(int param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 *puVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  undefined4 *puVar3;
  byte bVar4;
  
  bVar4 = 0;
  FUN_000749a0();
  iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x3c))();
  if ((((iVar1 != 0) && (iVar1 = FUN_00052eb0(), iVar1 != 0)) &&
      (*(int *)(extraout_ECX_00 + 0x5c) == 0x1f)) && (*(int *)(param_1 + 0xa9) != 0)) {
    iVar1 = *(int *)(param_1 + 0x30);
    if (iVar1 != 0) {
      iVar1 = iVar1 + 0x44;
    }
    if (*(int *)(extraout_ECX_00 + 0xa9) == iVar1) {
      *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x40;
    }
  }
  FUN_00053c90();
  puVar2 = (undefined4 *)FUN_00049580();
  *(undefined1 *)(param_2 + 3) = *(undefined1 *)(puVar2 + 3);
  puVar3 = param_2;
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
  return param_2;
}

