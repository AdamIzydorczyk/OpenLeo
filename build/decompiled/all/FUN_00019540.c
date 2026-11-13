
undefined8 __regparm3 FUN_00019540(byte param_1,undefined4 param_2)

{
  uint uVar1;
  
  out(0x20,0x20);
  uVar1 = (uint)param_1;
  if (7 < uVar1) {
    uVar1 = 0x20;
    out(0xa0,0x20);
  }
  return CONCAT44(param_2,uVar1);
}

