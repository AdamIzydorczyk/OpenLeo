
void __regparm3 FUN_00078bd7(int param_1)

{
  code *pcVar1;
  uint uVar2;
  
  if (param_1 != 0) {
    uVar2 = (param_1 * DAT_000cf9b8 + 500U) / 1000;
    if (uVar2 == 0) {
      uVar2 = 1;
    }
    do {
      pcVar1 = (code *)swi(0x21);
      (*pcVar1)();
      uVar2 = uVar2 - 1;
    } while (uVar2 != 0);
  }
  return;
}

