
uint __regparm3 FUN_00017fd7(uint param_1,uint param_2)

{
  uint uVar1;
  byte unaff_BL;
  
  uVar1 = (param_1 >> 9 & 0x7f | (param_2 & 0x1ff) << 7) + (param_2 >> 9) * 0x10000;
  if ((unaff_BL & 4) != 0) {
    uVar1 = uVar1 * 2 & 0x3ffff | uVar1 & 0xc0000;
  }
  return uVar1;
}

