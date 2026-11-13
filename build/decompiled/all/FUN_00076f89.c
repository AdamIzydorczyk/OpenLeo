
char * __regparm3 FUN_00076f89(char *param_1,char param_2)

{
  char cVar1;
  char *pcVar2;
  
  pcVar2 = (char *)0x0;
  do {
    if (param_2 == *param_1) {
      pcVar2 = param_1;
    }
    cVar1 = *param_1;
    param_1 = param_1 + 1;
  } while (cVar1 != '\0');
  return pcVar2;
}

