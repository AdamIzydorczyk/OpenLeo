
void __regparm3 FUN_0007c31b(char *param_1,int param_2)

{
  char cVar1;
  char *pcVar2;
  
  if ((*(byte *)(param_2 + 0x14) & 1) == 0) {
    for (; ((*param_1 != '\0' && (*param_1 != 'e')) && (*param_1 != 'E')); param_1 = param_1 + 1) {
    }
    pcVar2 = param_1 + -1;
    if ((*(char *)(param_2 + 0x15) == 'G') || (*(char *)(param_2 + 0x15) == 'g')) {
      for (; *pcVar2 == '0'; pcVar2 = pcVar2 + -1) {
      }
    }
    if (*pcVar2 != '.') {
      pcVar2 = pcVar2 + 1;
    }
    do {
      cVar1 = *param_1;
      param_1 = param_1 + 1;
      *pcVar2 = cVar1;
      pcVar2 = pcVar2 + 1;
    } while (cVar1 != '\0');
  }
  return;
}

