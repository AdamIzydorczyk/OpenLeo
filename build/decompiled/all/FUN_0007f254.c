
void __regparm3 FUN_0007f254(uint *param_1)

{
  undefined2 uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  ushort uVar6;
  short sVar7;
  undefined4 *puVar8;
  undefined1 *puVar9;
  undefined4 uVar10;
  undefined4 *puVar11;
  bool bVar12;
  bool bVar13;
  undefined8 uVar14;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  
  puVar9 = (undefined1 *)((int)register0x00000010 + -0x18);
  puVar8 = (undefined4 *)((int)register0x00000010 + -0x18);
  uVar10 = 0;
  *(byte *)((int)param_1 + 9) = *(byte *)((int)param_1 + 9) & 0x7f;
  bVar13 = false;
  if (((short)param_1[2] == 0) && (bVar13 = false, param_1[1] == 0)) {
    bVar13 = *param_1 == 0;
  }
  if (bVar13) {
    iVar4 = 0;
  }
  else {
    if ((short)param_1[2] < 0) {
      uVar10 = 0xffffffff;
      uStack_1c = 0xbfff;
      uStack_20 = 0x80000000;
      uStack_24 = 0;
      FUN_0007dee2();
      puVar8 = &uStack_24;
      *(byte *)((int)param_1 + 9) = *(byte *)((int)param_1 + 9) ^ 0x80;
    }
    iVar4 = 0;
    uVar5 = param_1[1];
    uVar3 = *param_1;
    for (uVar6 = (short)param_1[2] + 4; 0x3ffe < uVar6; uVar6 = uVar6 - 1) {
      bVar13 = CARRY4(uVar3,uVar3);
      uVar3 = uVar3 * 2;
      bVar12 = CARRY4(uVar5,uVar5);
      uVar2 = uVar5 * 2;
      uVar5 = uVar2 + bVar13;
      iVar4 = iVar4 * 2 + (uint)(bVar12 || CARRY4(uVar2,(uint)bVar13));
    }
    if (uVar5 == 0 && uVar3 == 0) {
      sVar7 = 0;
    }
    else {
      for (; -1 < (int)uVar5; uVar5 = uVar5 * 2 + (uint)bVar13) {
        bVar13 = CARRY4(uVar3,uVar3);
        uVar3 = uVar3 * 2;
        uVar6 = uVar6 - 1;
      }
      sVar7 = uVar6 - 4;
    }
    *param_1 = uVar3;
    param_1[1] = uVar5;
    *(short *)(param_1 + 2) = sVar7;
    puVar9 = (undefined1 *)puVar8;
  }
  *(undefined4 *)(puVar9 + -4) = uVar10;
  *(int *)(puVar9 + -8) = iVar4;
  uVar5 = param_1[2];
  uVar3 = param_1[2];
  *(undefined4 *)(puVar9 + -0xc) = 0x7f2fb;
  uVar14 = FUN_0007e316();
  *(int *)(puVar9 + -0xc) = (int)uVar14;
  *(int *)(puVar9 + -0x10) = (int)((ulonglong)uVar14 >> 0x20);
  *(uint *)(puVar9 + -0x14) = CONCAT22((short)uVar5,(ushort)uVar3);
  *(undefined4 *)(puVar9 + -0x18) = 0x7f319;
  uVar14 = FUN_0007df12();
  LOCK();
  *(int *)(puVar9 + -0xc) = (int)uVar14;
  UNLOCK();
  LOCK();
  *(int *)(puVar9 + -0x10) = (int)((ulonglong)uVar14 >> 0x20);
  UNLOCK();
  LOCK();
  uVar5 = *(uint *)(puVar9 + -0x14);
  *(uint *)(puVar9 + -0x14) = (ushort)uVar3 | 0x40030000;
  UNLOCK();
  *(undefined4 *)(puVar9 + -0x18) = 0x7f33f;
  FUN_0007e316();
  *(undefined4 *)(puVar9 + -0x18) = 0x7f35a;
  FUN_0007df12();
  uVar3 = param_1[2];
  *(undefined4 *)(puVar9 + -0x18) = 0x7f36e;
  uVar14 = FUN_0007e316();
  *(int *)(puVar9 + -0x18) = (int)uVar14;
  *(int *)(puVar9 + -0x1c) = (int)((ulonglong)uVar14 >> 0x20);
  *(uint *)(puVar9 + -0x20) = (uint)(ushort)uVar3 << 0x10 | uVar5 & 0xffff;
  uVar6 = (ushort)uVar5;
  uVar1 = *(undefined2 *)(puVar9 + -0x14);
  *(undefined4 *)(puVar9 + -0x24) = 0x7f38c;
  uVar14 = FUN_0007df12();
  LOCK();
  *(int *)(puVar9 + -0x18) = (int)uVar14;
  UNLOCK();
  LOCK();
  *(int *)(puVar9 + -0x1c) = (int)((ulonglong)uVar14 >> 0x20);
  UNLOCK();
  LOCK();
  uVar5 = *(uint *)(puVar9 + -0x20);
  *(uint *)(puVar9 + -0x20) = CONCAT22(uVar6,uVar1) << 0x10 | (uint)(uVar6 ^ 0x8000);
  UNLOCK();
  *(undefined4 *)(puVar9 + -0x24) = 0x7f3aa;
  FUN_0007df12();
  *(undefined4 *)(puVar9 + -0x18) = 0x7f3bc;
  uVar14 = FUN_0007e141();
  *(undefined8 *)param_1 = uVar14;
  *(short *)(param_1 + 2) = (short)uVar5;
  uVar5 = *(uint *)(puVar9 + -8);
  puVar11 = &DAT_0007f22c;
  while (uVar5 != 0) {
    uVar3 = uVar5 & 1;
    uVar5 = uVar5 >> 1;
    if (uVar3 != 0) {
      *(uint *)(puVar9 + -8) = uVar5;
      *(undefined4 **)(puVar9 + -0xc) = puVar11;
      uVar1 = *(undefined2 *)(puVar11 + 2);
      *(undefined4 *)(puVar9 + -0x10) = 0x7f3f7;
      uVar14 = FUN_0007e316();
      *(undefined8 *)param_1 = uVar14;
      *(undefined2 *)(param_1 + 2) = uVar1;
      puVar11 = *(undefined4 **)(puVar9 + -0xc);
      uVar5 = *(uint *)(puVar9 + -8);
    }
    puVar11 = (undefined4 *)((int)puVar11 + 10);
  }
  *(short *)(param_1 + 2) = (short)param_1[2] + (short)*(undefined4 *)(puVar9 + -4);
  *(undefined4 *)(puVar9 + -4) = 0xbfff;
  *(undefined4 *)(puVar9 + -8) = 0x80000000;
  *(undefined4 *)(puVar9 + -0xc) = 0;
  *(undefined4 *)(puVar9 + -0x10) = 0x7f421;
  FUN_0007dee2();
  return;
}

