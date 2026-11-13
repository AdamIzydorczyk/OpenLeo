
int __regparm3 FUN_00057dd0(int *param_1,char param_2)

{
  bool bVar1;
  char *pcVar2;
  char *pcVar3;
  
  if (((char *)*param_1 == (char *)0x0) || (*(char *)*param_1 == '\0')) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  if (bVar1) {
    return -1;
  }
  pcVar3 = (char *)*param_1;
  do {
    pcVar2 = pcVar3;
    if (*pcVar3 == param_2) goto LAB_00057e0a;
    if (*pcVar3 == '\0') break;
    pcVar2 = pcVar3 + 1;
    if (*pcVar2 == param_2) goto LAB_00057e0a;
    pcVar3 = pcVar3 + 2;
  } while (*pcVar2 != '\0');
  pcVar2 = (char *)0x0;
LAB_00057e0a:
  if (pcVar2 == (char *)0x0) {
    return -1;
  }
  return (int)pcVar2 - *param_1;
}

