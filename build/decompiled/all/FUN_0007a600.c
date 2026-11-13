
longlong FUN_0007a600(void)

{
  code *pcVar1;
  uint in_ECX;
  byte in_CF;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((in_CF & 1) == 0) {
    FUN_0007f9ca();
    return (ulonglong)in_ECX << 0x20;
  }
  FUN_00078c12();
  return CONCAT44(in_ECX,0xffffffff);
}

