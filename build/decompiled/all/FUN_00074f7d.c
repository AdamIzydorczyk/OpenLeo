
undefined4 __regparm3 FUN_00074f7d(undefined4 param_1,char *param_2)

{
  char cVar1;
  code *pcVar2;
  undefined4 uVar3;
  
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  do {
    cVar1 = *param_2;
    param_2 = param_2 + 1;
  } while (cVar1 != '\0');
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  FUN_00078b55();
  pcVar2 = (code *)swi(0x21);
  uVar3 = (*pcVar2)();
  return uVar3;
}

