
int __regparm3 FUN_00053440(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar2 = 0;
  do {
    if (*(int *)(param_1 + 0x44) <= iVar2) {
      return 0;
    }
    iVar2 = iVar2 + 1;
    iVar1 = FUN_00076a69();
  } while (iVar1 != 0);
  return iVar2;
}

