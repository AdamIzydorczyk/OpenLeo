
undefined8 __regparm3 FUN_00011610(char param_1)

{
  undefined4 uVar1;
  
  if (param_1 == '\x01') {
    DAT_000ade97 = DAT_000ade97 | 4;
    DAT_000ade98 = DAT_000ade98 | 1;
  }
  else {
    DAT_000ade97 = DAT_000ade97 | 8;
    DAT_000ade98 = DAT_000ade98 | 2;
  }
  FUN_000105dc();
  uVar1 = FUN_000105dc();
  out(DAT_000ade94 + 8,4);
  out((short)(DAT_000ade94 + 9),DAT_000ade98);
  return CONCAT44(DAT_000ade94 + 9,CONCAT31((int3)((uint)uVar1 >> 8),DAT_000ade98));
}

