
undefined8 __regparm3 FUN_00040920(int param_1,undefined4 param_2)

{
  uint uVar1;
  
  uVar1 = (**(code **)(**(int **)(param_1 + 5) + 0x2c))();
  if (uVar1 != 0xffffffff) {
    if ((uVar1 & 1) != 0) {
      return CONCAT44(param_2,2);
    }
    if ((uVar1 & 2) != 0) {
      return CONCAT44(param_2,5);
    }
    if ((uVar1 & 4) != 0) {
      return CONCAT44(param_2,7);
    }
    if ((uVar1 & 8) != 0) {
      return CONCAT44(param_2,10);
    }
  }
  return CONCAT44(param_2,0xffffffff);
}

