
undefined4 __regparm3 FUN_0007f682(int param_1,undefined4 param_2,undefined4 param_3)

{
  code *pcVar1;
  
  if (DAT_000ab02d != '\0') {
    pcVar1 = (code *)swi(0x2f);
    (*pcVar1)();
    pcVar1 = (code *)swi(0x2f);
    (*pcVar1)();
    return 1;
  }
  if (DAT_000ab02c != '\0') {
    *(undefined1 *)(param_1 + 0x3e) = 0;
    *(byte *)(param_1 + 0x40) = *(byte *)(param_1 + 0x40) & 0xfb;
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)(param_3);
  }
  return 0;
}

