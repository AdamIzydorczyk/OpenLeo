
char * __regparm3 FUN_0008050c(char *param_1,char *param_2,int param_3)

{
  char cVar1;
  int unaff_EBX;
  
  if ((param_3 == 0) && (unaff_EBX < 1)) {
    *param_1 = '0';
    param_1[1] = '.';
    return param_1 + 2;
  }
  for (; (0 < unaff_EBX && (*param_2 != '\0')); param_2 = param_2 + 1) {
    unaff_EBX = unaff_EBX + -1;
    *param_1 = *param_2;
    param_1 = param_1 + 1;
  }
  for (; 0 < unaff_EBX; unaff_EBX = unaff_EBX + -1) {
    *param_1 = '0';
    param_1 = param_1 + 1;
  }
  *param_1 = '.';
  param_1 = param_1 + 1;
  if (0 < param_3) {
    do {
      if (unaff_EBX == 0) break;
      unaff_EBX = unaff_EBX + 1;
      *param_1 = '0';
      param_1 = param_1 + 1;
      param_3 = param_3 + -1;
    } while (param_3 != 0);
  }
  if (0 < param_3) {
    do {
      cVar1 = *param_2;
      if (cVar1 == '\0') break;
      param_2 = param_2 + 1;
      *param_1 = cVar1;
      param_1 = param_1 + 1;
      param_3 = param_3 + -1;
    } while (param_3 != 0);
    for (; param_3 != 0; param_3 = param_3 + -1) {
      *param_1 = '0';
      param_1 = param_1 + 1;
    }
  }
  return param_1;
}

