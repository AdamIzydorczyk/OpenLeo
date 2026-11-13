
void FUN_00075c5a(void)

{
  code *pcVar1;
  undefined4 in_ECX;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)(in_ECX);
  return;
}

