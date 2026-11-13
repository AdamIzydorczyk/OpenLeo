
void __regparm3 FUN_00011e78(undefined4 param_1,undefined4 param_2)

{
  byte bVar1;
  
  out(DAT_000ae644 + 4,0xe);
  FUN_00011e10(param_2);
  bVar1 = in(DAT_000ae644 + 5);
  out(DAT_000ae644 + 5,bVar1 & 0xfd);
  FUN_00011e10();
  return;
}

