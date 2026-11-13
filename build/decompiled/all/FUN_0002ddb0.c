
int __regparm3 FUN_0002ddb0(int *param_1,int *param_2)

{
  int iVar1;
  
  iVar1 = 0;
  while ((((*param_2 < *param_1 || (param_1[2] < *param_2)) || (param_2[1] < param_1[1])) ||
         (param_1[3] < param_2[1]))) {
    iVar1 = iVar1 + 1;
    param_1 = param_1 + 7;
    if (2 < iVar1) {
      return -1;
    }
  }
  return iVar1;
}

