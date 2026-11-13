
longlong __regparm3 FUN_00040970(int param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = (**(code **)(**(int **)(param_1 + 5) + 0x30))();
  if (uVar1 != 0xffffffff) {
    if ((uVar1 & 1) != 0) {
      return (ulonglong)param_2 << 0x20;
    }
    if ((uVar1 & 2) != 0) {
      return CONCAT44(param_2,1);
    }
    if ((uVar1 & 8) != 0) {
      return CONCAT44(param_2,3);
    }
  }
  return CONCAT44(param_2,0xffffffff);
}

