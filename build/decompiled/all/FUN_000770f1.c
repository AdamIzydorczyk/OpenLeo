
undefined8 __regparm3 FUN_000770f1(undefined4 *param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 *extraout_ECX;
  undefined4 *extraout_ECX_00;
  undefined4 *extraout_ECX_01;
  undefined4 *extraout_ECX_02;
  undefined4 *puVar4;
  
  if (param_1[2] == 0) {
    FUN_0007a448();
    param_1 = extraout_ECX;
  }
  if (((*(byte *)((int)param_1 + 0xd) & 0x20) != 0) && ((*(byte *)((int)param_1 + 0xd) & 6) != 0)) {
    FUN_0007770d();
    param_1 = extraout_ECX_00;
  }
  *param_1 = param_1[2];
  uVar1 = param_1[3];
  *(byte *)(param_1 + 3) = *(byte *)(param_1 + 3) & 0xfb;
  if (((uVar1 & 0x2400) == 0x2400) && (param_1[4] == 0)) {
    param_1[1] = 0;
    iVar2 = FUN_0007b913();
    puVar4 = extraout_ECX_01;
    if (iVar2 != -1) {
      *(char *)*extraout_ECX_01 = (char)iVar2;
      extraout_ECX_01[1] = 1;
    }
  }
  else {
    uVar3 = FUN_0007a4bf();
    extraout_ECX_02[1] = uVar3;
    puVar4 = extraout_ECX_02;
  }
  if ((int)puVar4[1] < 1) {
    if (puVar4[1] == 0) {
      *(byte *)(puVar4 + 3) = *(byte *)(puVar4 + 3) | 0x10;
    }
    else {
      puVar4[1] = 0;
      *(byte *)(puVar4 + 3) = *(byte *)(puVar4 + 3) | 0x20;
    }
  }
  return CONCAT44(param_2,puVar4[1]);
}

