
void __regparm3 FUN_0007e9ea(uint *param_1,undefined4 param_2,undefined4 param_3)

{
  uint uVar1;
  uint uVar2;
  char cVar3;
  char extraout_CL;
  char extraout_CL_00;
  undefined2 extraout_CX;
  byte bVar5;
  byte bVar4;
  byte extraout_CH;
  undefined4 *puVar6;
  bool bVar7;
  bool bVar8;
  undefined8 uVar9;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  
  bVar4 = *(byte *)((int)param_1 + 9);
  uStack_18 = CONCAT31((int3)((uint)param_3 >> 8),bVar4) & 0xffff00ff;
  *(byte *)((int)param_1 + 9) = *(byte *)((int)param_1 + 9) & 0x7f;
  if ((((short)param_1[2] == 0x3fff) && (param_1[1] == 0x80000000)) && (*param_1 == 0)) {
    *param_1 = 0x2168c235;
    param_1[1] = 0xc90fdaa2;
    *(ushort *)(param_1 + 2) = CONCAT11(bVar4 & 0x80 | 0x3f,0xfe);
    return;
  }
  if (0x3ffe < (short)param_1[2]) {
    uStack_1c = 0x7ea56;
    uVar9 = FUN_0007e141();
    *(undefined8 *)param_1 = uVar9;
    *(undefined2 *)(param_1 + 2) = 0x3fff;
    uStack_18 = CONCAT22((short)(uStack_18 >> 0x10),CONCAT11(2,(char)uStack_18));
  }
  bVar7 = (ushort)param_1[2] < 0x3ffd;
  bVar8 = false;
  if ((ushort)param_1[2] == 0x3ffd) {
    bVar7 = param_1[1] < 0x8930a2f4;
    bVar8 = false;
    if (param_1[1] == 0x8930a2f4) {
      bVar7 = *param_1 < 0xf66ab09b;
      bVar8 = *param_1 == 0xf66ab09b;
    }
  }
  if (!bVar7 && !bVar8) {
    uVar1 = param_1[2];
    uStack_1c = 0x7ea9d;
    uVar9 = FUN_0007df12();
    uStack_20 = (undefined4)((ulonglong)uVar9 >> 0x20);
    uStack_1c = (undefined4)uVar9;
    uVar2 = param_1[2];
    FUN_0007e316(CONCAT22(0x3fff,(short)uVar1));
    FUN_0007df12();
    uStack_1c = 0x7eae3;
    uVar9 = FUN_0007e141();
    *(undefined8 *)param_1 = uVar9;
    *(short *)(param_1 + 2) = (short)uVar2;
  }
  uStack_18 = 0x7eb02;
  FUN_0007f566();
  puVar6 = (undefined4 *)((int)register0x00000010 + -0x14);
  bVar5 = (byte)((ushort)extraout_CX >> 8);
  cVar3 = (char)extraout_CX;
  bVar4 = bVar5 >> 1;
  if ((bool)(bVar5 & 1)) {
    uStack_18 = 0x3ffe;
    uStack_1c = 0x860a91c1;
    uStack_20 = 0x6b9b2c23;
    FUN_0007dee2();
    puVar6 = &uStack_20;
    cVar3 = extraout_CL;
    bVar4 = extraout_CH;
  }
  if ((bool)(bVar4 & 1)) {
    *(undefined4 *)((int)puVar6 + -4) = 0xbfff;
    *(undefined4 *)((int)puVar6 + -8) = 0xc90fdaa2;
    *(undefined4 *)((int)puVar6 + -0xc) = 0x2168c235;
    *(undefined4 *)((int)puVar6 + -0x10) = 0x7eb3a;
    FUN_0007dee2();
    *(byte *)((int)param_1 + 9) = *(byte *)((int)param_1 + 9) ^ 0x80;
    cVar3 = extraout_CL_00;
  }
  if (cVar3 < '\0') {
    *(byte *)((int)param_1 + 9) = *(byte *)((int)param_1 + 9) ^ 0x80;
  }
  return;
}

