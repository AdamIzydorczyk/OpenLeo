
longlong __regparm3 FUN_0001f590(int param_1,uint param_2)

{
  if (*(int *)(param_1 + 0x122) == 1) {
    return CONCAT44(param_2,1);
  }
  if ((*(int *)(param_1 + 0x122) == 2) && (DAT_000cebf4 == 1)) {
    return CONCAT44(param_2,1);
  }
  if ((*(int *)(param_1 + 0x122) == 3) && ((DAT_000cebf0 == 1 && (DAT_000cebf4 == 2)))) {
    return CONCAT44(param_2,1);
  }
  return (ulonglong)param_2 << 0x20;
}

