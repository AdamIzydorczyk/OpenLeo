
undefined8 __regparm3 FUN_0007c231(char *param_1,int *param_2)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  byte bVar4;
  int unaff_EBX;
  uint uVar5;
  undefined4 uVar6;
  ushort in_DS;
  
  uVar5 = (uint)in_DS;
  *(undefined4 *)(unaff_EBX + 0xc) = 0;
  bVar4 = *(byte *)(unaff_EBX + 0x15);
  uVar6 = 0;
  if (bVar4 < 0x47) {
    if (bVar4 < 0x45) goto LAB_0007c30f;
    if (bVar4 < 0x46) goto LAB_0007c28d;
  }
  else {
    if (bVar4 < 0x48) {
LAB_0007c27d:
      if (*(int *)(unaff_EBX + 8) == 0) {
        *(undefined4 *)(unaff_EBX + 8) = 1;
      }
      bVar4 = bVar4 - 2;
    }
    else {
      if (0x65 < bVar4) {
        if (bVar4 < 0x67) goto LAB_0007c292;
        if (bVar4 != 0x67) goto LAB_0007c30f;
        goto LAB_0007c27d;
      }
      if (bVar4 != 0x65) goto LAB_0007c30f;
    }
LAB_0007c28d:
    uVar6 = 1;
  }
LAB_0007c292:
  puVar1 = (undefined4 *)*param_2;
  *param_2 = (int)(puVar1 + 2);
  uVar2 = *puVar1;
  uVar3 = puVar1[1];
  if (*(int *)(unaff_EBX + 8) == -1) {
    *(undefined4 *)(unaff_EBX + 8) = 6;
  }
  FUN_000805e0(0xff,uVar6,3,bVar4,*(byte *)(unaff_EBX + 0x15) & 0x5f,uVar2,uVar3);
  if (*param_1 == '*') {
    *(undefined1 *)(unaff_EBX + 0x16) = 0x2a;
  }
  param_1[0xff] = '\0';
  for (; *param_1 == ' '; param_1 = param_1 + 1) {
  }
  FUN_0007c31b();
  uVar5 = (uint)in_DS;
  *(undefined4 *)(unaff_EBX + 8) = 0xffffffff;
LAB_0007c30f:
  return CONCAT44(uVar5,param_1);
}

