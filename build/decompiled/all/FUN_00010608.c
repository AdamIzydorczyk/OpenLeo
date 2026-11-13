
ulonglong __regparm3 FUN_00010608(undefined1 param_1,undefined4 param_2)

{
  DAT_000adea8 = param_1;
  out(DAT_000ade94 + 0x103,param_1);
  out((short)(DAT_000ade94 + 0x104),(short)param_2);
  return CONCAT44(DAT_000ade94 + 0x104,param_2) & 0xffffffff0000ffff;
}

