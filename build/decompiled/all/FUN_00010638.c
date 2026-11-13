
undefined8 __regparm3 FUN_00010638(undefined1 param_1,undefined4 param_2)

{
  byte bVar1;
  
  DAT_000adea8 = param_1;
  out(DAT_000ade94 + 0x103,param_1);
  bVar1 = in(DAT_000ade94 + 0x105);
  return CONCAT44(param_2,(uint)bVar1);
}

