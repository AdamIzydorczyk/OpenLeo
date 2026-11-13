
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00079d24(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  _DAT_000a9e18 = (uint)DAT_000a9e5e;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}

