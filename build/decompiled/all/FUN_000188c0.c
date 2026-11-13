
void __regparm3 FUN_000188c0(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  if (DAT_000c3fe4 != 0) {
    out(0x43,0x36);
    out(0x40,0);
    out(0x40,0);
    FUN_00075c8c(param_2,param_3);
    DAT_000c3fe4 = 0;
  }
  return;
}

