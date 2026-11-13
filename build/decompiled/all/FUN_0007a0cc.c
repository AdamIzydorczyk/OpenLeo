
undefined8 FUN_0007a0cc(char *param_1,uint param_2,uint param_3)

{
  code *pcVar1;
  uint uVar2;
  bool bVar3;
  int iVar4;
  uint *puVar5;
  uint *puVar6;
  uint *puVar7;
  uint uVar8;
  byte bVar9;
  ulonglong uVar10;
  uint auStack_20 [2];
  uint local_18;
  undefined4 local_8;
  
  for (; *param_1 == ' '; param_1 = param_1 + 1) {
  }
  auStack_20[1] = param_2 & 0x83;
  bVar9 = 0;
  uVar8 = 0xffffffff;
  pcVar1 = (code *)swi(0x21);
  iVar4 = (*pcVar1)();
  bVar3 = (bVar9 & 1) != 0;
  uVar2 = (iVar4 << 1 | (uint)bVar9) >> 1;
  auStack_20[1] = uVar2 | (uint)bVar3 << 0x1f;
  if (!bVar3) {
    uVar8 = uVar2 & 0xffff;
  }
  puVar6 = auStack_20 + 1;
  if (((param_3 & 3) == 0) || (puVar6 = auStack_20 + 1, uVar8 == 0xffffffff)) {
LAB_0007a199:
    puVar7 = puVar6;
    if (uVar8 != 0xffffffff) goto LAB_0007a23b;
    puVar5 = puVar6;
    if (((*(byte *)((int)puVar6 + 0x28) & 0x20) != 0) && (*(short *)puVar6 == 2)) {
      bVar9 = 0;
      pcVar1 = (code *)swi(0x21);
      iVar4 = (*pcVar1)();
      puVar5 = (uint *)((int)puVar6 + 4);
      puVar7 = (uint *)((int)puVar6 + 4);
      bVar3 = (bVar9 & 1) != 0;
      *(uint *)((int)puVar6 + 4) = (iVar4 << 1 | (uint)bVar9) >> 1 | (uint)bVar3 << 0x1f;
      if (!bVar3) {
        uVar8 = (uint)*(ushort *)((int)puVar6 + 4);
        bVar9 = 0;
        if (*(int *)((int)puVar6 + 0x30) == 0) {
LAB_0007a23b:
          *(undefined4 *)((int)puVar7 + -4) = 0x7a242;
          FUN_0007f975();
          *(undefined4 *)((int)puVar7 + -4) = 0x7a24d;
          FUN_0007f94e();
          *(byte *)((int)puVar7 + 4) = *(byte *)((int)puVar7 + 4) & 0x7f;
          *(undefined4 *)((int)puVar7 + -4) = 0x7a2aa;
          FUN_0007f9ca();
          goto LAB_0007a2ac;
        }
        pcVar1 = (code *)swi(0x21);
        iVar4 = (*pcVar1)();
        puVar5 = (uint *)((int)puVar6 + 8);
        bVar3 = (bVar9 & 1) != 0;
        *(uint *)((int)puVar6 + 8) = (iVar4 << 1 | (uint)bVar9) >> 1 | (uint)bVar3 << 0x1f;
        if (!bVar3) {
          bVar9 = 0;
          pcVar1 = (code *)swi(0x21);
          iVar4 = (*pcVar1)();
          puVar5 = (uint *)((int)puVar6 + 0xc);
          puVar7 = (uint *)((int)puVar6 + 0xc);
          bVar3 = (bVar9 & 1) != 0;
          *(uint *)((int)puVar6 + 0xc) = (iVar4 << 1 | (uint)bVar9) >> 1 | (uint)bVar3 << 0x1f;
          if (!bVar3) {
            uVar8 = (uint)*(ushort *)((int)puVar6 + 0xc);
            goto LAB_0007a23b;
          }
        }
      }
    }
  }
  else {
    uVar10 = FUN_0007f94e();
    puVar6 = auStack_20 + 1;
    if ((int)uVar10 != 0) goto LAB_0007a199;
    if (((param_3 & 0x400) != 0) && ((uVar10 & 0x2000000000) != 0)) {
      pcVar1 = (code *)swi(0x21);
      (*pcVar1)();
      auStack_20[1] = 0x7a157;
      FUN_00078c12();
      uVar8 = 0xffffffff;
      puVar7 = &local_18;
      goto LAB_0007a2ac;
    }
    puVar6 = auStack_20 + 1;
    if ((param_3 & 0x40) == 0) goto LAB_0007a199;
    bVar9 = 0;
    pcVar1 = (code *)swi(0x21);
    iVar4 = (*pcVar1)();
    puVar6 = &local_18;
    bVar3 = (bVar9 & 1) != 0;
    local_18 = (iVar4 << 1 | (uint)bVar9) >> 1 | (uint)bVar3 << 0x1f;
    if (!bVar3) goto LAB_0007a199;
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    puVar5 = auStack_20 + 3;
  }
  *(undefined4 *)((int)puVar5 + -4) = 0x7a194;
  uVar8 = FUN_0007ba5f();
  puVar7 = puVar5;
LAB_0007a2ac:
  return CONCAT44(*(undefined4 *)((int)puVar7 + 0x14),uVar8);
}

