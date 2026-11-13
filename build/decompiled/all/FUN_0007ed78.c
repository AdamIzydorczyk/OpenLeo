
void __regparm3 FUN_0007ed78(uint *param_1)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  bool bVar5;
  bool bVar6;
  undefined8 uVar7;
  undefined2 uVar8;
  undefined2 uVar9;
  
  iVar2 = (ushort)param_1[2] - 0x3ffe;
  *(undefined2 *)(param_1 + 2) = 0x3ffe;
  uVar7 = FUN_0007df12();
  bVar5 = (ushort)param_1[2] < 0x3ffe;
  bVar6 = false;
  if ((ushort)param_1[2] == 0x3ffe) {
    bVar5 = param_1[1] < 0xb504f333;
    bVar6 = false;
    if (param_1[1] == 0xb504f333) {
      bVar5 = *param_1 < 0xf9de6484;
      bVar6 = *param_1 == 0xf9de6484;
    }
  }
  if (bVar5 || bVar6) {
    *(undefined8 *)param_1 = uVar7;
    *(undefined2 *)(param_1 + 2) = 0x3ffe;
    iVar2 = iVar2 + -1;
  }
  else {
    uVar7 = FUN_0007df12();
  }
  uVar8 = 0x3ffe;
  uVar4 = param_1[2];
  FUN_0007df12(0xbffe3ffe,(int)((ulonglong)uVar7 >> 0x20),(int)uVar7);
  uVar3 = CONCAT22((short)uVar4 + -1,uVar8);
  uVar7 = FUN_0007e141();
  uVar7 = FUN_0007e316(uVar3,(int)((ulonglong)uVar7 >> 0x20),(int)uVar7);
  uVar9 = (undefined2)uVar3;
  *(undefined8 *)param_1 = uVar7;
  *(undefined2 *)(param_1 + 2) = uVar8;
  uVar7 = FUN_0007f514();
  uVar4 = param_1[2];
  FUN_0007f514(CONCAT22(uVar8,uVar8),(int)((ulonglong)uVar7 >> 0x20),(int)uVar7);
  FUN_0007e141();
  FUN_0007e316();
  FUN_0007e316();
  uVar4 = CONCAT22((ushort)uVar4,uVar9) << 0x10 | (uint)(ushort)uVar4;
  uVar7 = FUN_0007df12();
  if (iVar2 != 0) {
    FUN_0007e4d6(uVar4,(int)((ulonglong)uVar7 >> 0x20),(int)uVar7);
    uVar8 = (undefined2)uVar4;
    uVar4 = param_1[2];
    FUN_0007e316();
    uVar7 = FUN_0007df12();
    uVar1 = param_1[2];
    FUN_0007e316(CONCAT22((ushort)uVar4,uVar8) << 0x10 | (uint)(ushort)uVar4,
                 (int)((ulonglong)uVar7 >> 0x20),(int)uVar7);
    uVar4 = (uint)(ushort)uVar1;
    uVar7 = FUN_0007df12();
  }
  *(undefined8 *)param_1 = uVar7;
  *(short *)(param_1 + 2) = (short)uVar4;
  return;
}

