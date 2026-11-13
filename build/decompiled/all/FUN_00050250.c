
undefined8 __regparm3 FUN_00050250(int param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x68))();
  return CONCAT44(param_2,*(int *)(param_1 + 0xfa) +
                          *(int *)(iVar1 + 4) + (*(int *)(param_1 + 0xc6) * 0xf) / 100);
}

