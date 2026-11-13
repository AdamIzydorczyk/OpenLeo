
int __regparm3 FUN_00057e30(int *param_1)

{
  bool bVar1;
  int iVar2;
  
  if (((char *)*param_1 == (char *)0x0) || (*(char *)*param_1 == '\0')) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  if (bVar1) {
    return -1;
  }
  iVar2 = FUN_00076f89();
  if (iVar2 == 0) {
    return -1;
  }
  return iVar2 - *param_1;
}

