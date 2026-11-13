
undefined8 __regparm3 FUN_00050200(int param_1,undefined4 param_2)

{
  int *piVar1;
  
  piVar1 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x68))();
  return CONCAT44(param_2,*(int *)(param_1 + 0xfe) +
                          *piVar1 + (*(int *)(param_1 + 0xc2) * 0xf) / 100 +
                          *(int *)(param_1 + 0x6d) * 0x3c);
}

