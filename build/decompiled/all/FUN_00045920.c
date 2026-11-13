
undefined4 * __regparm3 FUN_00045920(undefined4 param_1,undefined4 *param_2)

{
  undefined4 *puVar1;
  uint extraout_ECX;
  uint uVar2;
  undefined4 *puVar3;
  byte bVar4;
  
  bVar4 = 0;
  FUN_00045870();
  puVar1 = (undefined4 *)FUN_00049580();
  *(undefined1 *)(param_2 + 3) = *(undefined1 *)(puVar1 + 3);
  puVar3 = param_2;
  for (uVar2 = extraout_ECX >> 2; uVar2 != 0; uVar2 = uVar2 - 1) {
    *puVar3 = *puVar1;
    puVar1 = puVar1 + (uint)bVar4 * -2 + 1;
    puVar3 = puVar3 + (uint)bVar4 * -2 + 1;
  }
  for (uVar2 = extraout_ECX & 3; uVar2 != 0; uVar2 = uVar2 - 1) {
    *(undefined1 *)puVar3 = *(undefined1 *)puVar1;
    puVar1 = (undefined4 *)((int)puVar1 + (uint)bVar4 * -2 + 1);
    puVar3 = (undefined4 *)((int)puVar3 + (uint)bVar4 * -2 + 1);
  }
  return param_2;
}

