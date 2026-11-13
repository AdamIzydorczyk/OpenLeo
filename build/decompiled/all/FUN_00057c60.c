
int __regparm3 FUN_00057c60(int *param_1,int param_2)

{
  int iVar1;
  
  if ((param_2 == 0) || (*param_1 == 0)) {
    return -1;
  }
  iVar1 = FUN_00076ea6();
  if (iVar1 == 0) {
    return -1;
  }
  return iVar1 - *param_1;
}

