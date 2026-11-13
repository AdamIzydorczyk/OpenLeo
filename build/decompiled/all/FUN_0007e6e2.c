
void __regparm3 FUN_0007e6e2(uint param_1,undefined4 *param_2)

{
  short sVar1;
  uint uVar2;
  ushort uVar4;
  ushort uVar3;
  undefined2 uVar6;
  uint uVar5;
  
  sVar1 = (short)(param_1 >> 0x10);
  uVar4 = sVar1 >> 7;
  uVar3 = uVar4 & 0xff;
  uVar2 = CONCAT22(sVar1 >> 0xf,uVar4) & 0xffff00ff;
  if (uVar3 != 0) {
    param_1 = param_1 << 8;
    uVar6 = (undefined2)(uVar2 >> 0x10);
    if ((char)uVar2 == -1) {
      uVar5 = CONCAT22(uVar6,0xffff);
      uVar2 = param_1 & 0x7fffffff;
      param_1 = 0;
      if (uVar2 != 0) {
        param_1 = uVar2 | 0x40000000;
      }
    }
    else {
      uVar5 = CONCAT22(uVar6,uVar3 + 0x3f80);
    }
    uVar2 = ((uint)CARRY4(uVar5,uVar5) << 0x10 | uVar5 * 2 & 0xfffe) >> 1;
    param_1 = param_1 | 0x80000000;
  }
  *param_2 = 0;
  param_2[1] = param_1;
  *(short *)(param_2 + 2) = (short)uVar2;
  return;
}

