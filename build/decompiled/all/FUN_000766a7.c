
int FUN_000766a7(void)

{
  code *pcVar1;
  char cVar2;
  
  if (DAT_000a9be0 == 0) {
    pcVar1 = (code *)swi(0x21);
    cVar2 = (*pcVar1)();
    return (int)cVar2;
  }
  return 1;
}

