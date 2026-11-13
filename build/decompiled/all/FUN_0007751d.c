
undefined4 __regparm3 FUN_0007751d(char *param_1)

{
  char cVar1;
  
  if (*param_1 != '\0') {
    for (; param_1[1] != '\0'; param_1 = param_1 + 1) {
    }
    cVar1 = *param_1;
    if (((cVar1 == '\\') || (cVar1 == '/')) || (cVar1 == '.')) {
      return 1;
    }
  }
  return 0;
}

