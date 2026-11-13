
void __regparm3 FUN_0007e51c(uint param_1,int param_2,undefined4 param_3)

{
  short sVar1;
  uint uVar2;
  uint uVar3;
  ushort uVar5;
  ushort uVar4;
  undefined2 uVar7;
  uint extraout_ECX;
  uint uVar6;
  uint uVar8;
  uint extraout_EDX;
  uint *unaff_EBX;
  bool bVar9;
  
  uVar8 = param_2 << 0xb | param_1 >> 0x15;
  uVar2 = param_1 << 0xb;
  sVar1 = (short)((uint)param_2 >> 0x10);
  uVar5 = sVar1 >> 4;
  uVar4 = uVar5 & 0x7ff;
  uVar3 = CONCAT22(sVar1 >> 0xf,uVar5) & 0xffff07ff;
  uVar7 = (undefined2)(uVar3 >> 0x10);
  if (uVar4 == 0) {
    uVar3 = uVar2;
    if ((uVar8 == 0) && (uVar2 == 0)) {
      uVar6 = 0;
    }
    else {
      uVar6 = CONCAT22(uVar7,0x3c01);
      if (uVar8 == 0) {
        uVar6 = CONCAT22(uVar7,0x3be1);
        uVar3 = 0;
        uVar8 = uVar2;
      }
      for (; -1 < (int)uVar8; uVar8 = uVar8 * 2 + (uint)bVar9) {
        bVar9 = CARRY4(uVar3,uVar3);
        uVar3 = uVar3 * 2;
        uVar6 = CONCAT22((short)(uVar6 >> 0x10),(short)uVar6 + -1);
      }
    }
  }
  else {
    if (uVar4 == 0x7ff) {
      uVar6 = CONCAT22(uVar7,0x7fff);
      if (((param_2 << 0xb & 0x7fffffffU) != 0 || param_1 >> 0x15 != 0) || uVar2 != 0) {
        FUN_0007c9a4(uVar2,param_3);
        uVar8 = extraout_EDX | 0x40000000;
        uVar6 = extraout_ECX;
      }
    }
    else {
      uVar6 = CONCAT22(uVar7,(short)uVar3 + 0x3c00);
    }
    uVar8 = uVar8 | 0x80000000;
    uVar3 = uVar2;
  }
  *unaff_EBX = uVar3;
  unaff_EBX[1] = uVar8;
  *(short *)(unaff_EBX + 2) = (short)(((uint)CARRY4(uVar6,uVar6) << 0x10 | uVar6 * 2 & 0xffff) >> 1)
  ;
  return;
}

