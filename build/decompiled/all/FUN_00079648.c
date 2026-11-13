
undefined8 __regparm3 FUN_00079648(int param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  puVar1 = *(undefined4 **)(param_1 + 8);
  *(undefined4 *)(param_1 + 8) = *puVar1;
  return CONCAT44(param_2,puVar1);
}

