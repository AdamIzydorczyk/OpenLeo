
longlong FUN_000776c6(void)

{
  code *pcVar1;
  undefined4 uVar2;
  byte in_CF;
  uint unaff_retaddr;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((in_CF & 1) == 0) {
    return (ulonglong)unaff_retaddr << 0x20;
  }
  uVar2 = FUN_0007ba5f();
  return CONCAT44(unaff_retaddr,uVar2);
}

