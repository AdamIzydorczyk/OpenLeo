
void FUN_0001646c(void)

{
  code *pcVar1;
  int iVar2;
  int extraout_EDX;
  
  iVar2 = 0;
  do {
    if (*(short *)((int)&DAT_000c3cf0 + iVar2) != 0) {
      pcVar1 = (code *)swi(0x31);
      (*pcVar1)();
      iVar2 = extraout_EDX;
    }
    iVar2 = iVar2 + 2;
  } while (iVar2 != 0x82);
  return;
}

