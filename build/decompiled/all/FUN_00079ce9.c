
void FUN_00079ce9(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ('\x02' < DAT_000a9bee) {
    FUN_00079d24();
  }
  return;
}

