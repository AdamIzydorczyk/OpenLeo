
undefined4 __regparm3 FUN_00081b76(uint param_1,undefined4 param_2,undefined4 param_3)

{
  code *pcVar1;
  byte bVar2;
  
  if (((int)param_1 < 0) || (bVar2 = param_1 < DAT_000ad504, DAT_000ad504 < param_1)) {
    FUN_00078c12();
    return 0xffffffff;
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)(param_3);
  if ((bVar2 & 1) == 0) {
    FUN_0007f9ca();
    return 0;
  }
  FUN_00078c12();
  return 0xffffffff;
}

