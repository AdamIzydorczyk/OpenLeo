
undefined8 __regparm3 FUN_00016a18(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  if (*(int **)(param_1 + 1) == (int *)0x0) {
    return CONCAT44(param_2,0xffffffff);
  }
  uVar1 = (**(code **)(**(int **)(param_1 + 1) + 0x24))();
  return CONCAT44(param_2,uVar1);
}

