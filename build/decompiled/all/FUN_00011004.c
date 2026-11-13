
undefined8 __regparm3 FUN_00011004(uint param_1,undefined4 param_2)

{
  uint uVar1;
  
  if (param_1 < 0x40001) {
    uVar1 = FUN_00010f70();
    if ((uVar1 != 0) && (uVar1 >> 0x12 != (uVar1 + (param_1 + 0x1f & 0xffffffe0)) - 1 >> 0x12)) {
      FUN_00010ee4();
      uVar1 = FUN_00011004();
      FUN_00010ee4();
    }
  }
  else {
    uVar1 = 0;
  }
  return CONCAT44(param_2,uVar1);
}

