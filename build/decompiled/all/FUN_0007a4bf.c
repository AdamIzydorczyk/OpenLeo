
void FUN_0007a4bf(void)

{
  code *pcVar1;
  byte in_CF;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((in_CF & 1) != 0) {
    FUN_0007ba5f();
  }
  return;
}

