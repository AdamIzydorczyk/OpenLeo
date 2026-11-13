
undefined8 FUN_0001dc0a(void)

{
  code *pcVar1;
  undefined4 in_ECX;
  bool bVar2;
  
  bVar2 = false;
  pcVar1 = (code *)swi(0x31);
  (*pcVar1)();
  return CONCAT44(in_ECX,(uint)!bVar2);
}

