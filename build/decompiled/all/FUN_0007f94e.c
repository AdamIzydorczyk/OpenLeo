
undefined8 FUN_0007f94e(void)

{
  code *pcVar1;
  undefined4 in_ECX;
  uint extraout_EDX;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return CONCAT44(in_ECX,(uint)((extraout_EDX & 0x80) != 0));
}

