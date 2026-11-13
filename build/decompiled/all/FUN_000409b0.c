
undefined8 __regparm3 FUN_000409b0(int param_1,undefined4 param_2)

{
  uint uVar1;
  
  uVar1 = (**(code **)(**(int **)(param_1 + 5) + 0x30))();
  if (uVar1 != 0xffffffff) {
    if ((uVar1 & 0x20) != 0) {
      return CONCAT44(param_2,5);
    }
    if ((uVar1 & 0x40) != 0) {
      return CONCAT44(param_2,6);
    }
    if ((uVar1 & 0x80) != 0) {
      return CONCAT44(param_2,7);
    }
  }
  return CONCAT44(param_2,0xffffffff);
}

