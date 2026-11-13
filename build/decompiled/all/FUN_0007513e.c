
undefined8 __regparm3 FUN_0007513e(byte param_1,undefined4 param_2)

{
  byte bVar1;
  uint uVar2;
  
  if ((0x2f < param_1) && (param_1 < 0x3a)) {
    return CONCAT44(param_2,param_1 - 0x30);
  }
  bVar1 = FUN_00078c41();
  if (((0x60 < bVar1) && (bVar1 < 0x6a)) || ((0x69 < bVar1 && (bVar1 < 0x73)))) {
    return CONCAT44(param_2,bVar1 - 0x57);
  }
  uVar2 = (uint)bVar1;
  if ((uVar2 < 0x73) || (0x7a < uVar2)) {
    return CONCAT44(param_2,0x25);
  }
  return CONCAT44(param_2,uVar2 - 0x57);
}

