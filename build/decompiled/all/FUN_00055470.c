
int __regparm3 FUN_00055470(int param_1,int param_2)

{
  int iVar1;
  int *piVar2;
  
  iVar1 = 0;
  if (0 < *(int *)(param_1 + 0x3c)) {
    piVar2 = *(int **)(param_1 + 0x40);
    do {
      if (param_2 == *piVar2) {
        return iVar1;
      }
      iVar1 = iVar1 + 1;
      piVar2 = piVar2 + 1;
    } while (iVar1 < *(int *)(param_1 + 0x3c));
  }
  return -1;
}

