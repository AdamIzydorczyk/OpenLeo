
undefined8 __regparm3 FUN_00017962(byte param_1,undefined4 param_2)

{
  byte bVar1;
  
  bVar1 = param_1 & 0x82 | 0x21;
  if (3 < DAT_000a8826) {
    bVar1 = param_1 & 0x82 | 0x25;
  }
  out((short)DAT_000a887f,0x49);
  out((short)DAT_000a8887,bVar1);
  return CONCAT44(param_2,(uint)bVar1);
}

