
undefined4 * __regparm3 FUN_00041990(int param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 *puVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  uint extraout_ECX_01;
  uint uVar3;
  int *unaff_EBX;
  undefined4 *puVar4;
  byte bVar5;
  
  bVar5 = 0;
  iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x70))();
  if ((float)iVar1 * *(float *)(extraout_ECX + 0x140) < 0.0) {
    FUN_00053670();
    *unaff_EBX = -*unaff_EBX;
  }
  unaff_EBX[2] = 1;
  FUN_000749a0();
  (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0xf0))();
  puVar2 = (undefined4 *)FUN_00049580();
  *(undefined1 *)(param_2 + 3) = *(undefined1 *)(puVar2 + 3);
  puVar4 = param_2;
  for (uVar3 = extraout_ECX_01 >> 2; uVar3 != 0; uVar3 = uVar3 - 1) {
    *puVar4 = *puVar2;
    puVar2 = puVar2 + (uint)bVar5 * -2 + 1;
    puVar4 = puVar4 + (uint)bVar5 * -2 + 1;
  }
  for (uVar3 = extraout_ECX_01 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
    *(undefined1 *)puVar4 = *(undefined1 *)puVar2;
    puVar2 = (undefined4 *)((int)puVar2 + (uint)bVar5 * -2 + 1);
    puVar4 = (undefined4 *)((int)puVar4 + (uint)bVar5 * -2 + 1);
  }
  return param_2;
}

