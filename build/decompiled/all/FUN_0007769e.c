
longlong __regparm3 FUN_0007769e(int param_1,uint param_2)

{
  if ((param_1 == 0) || (1 < *(byte *)(param_1 + 0x2d))) {
    return CONCAT44(param_2,1);
  }
  *(undefined1 *)(param_1 + 0x2d) = 2;
  FUN_00075302();
  return (ulonglong)param_2 << 0x20;
}

