
undefined8 __regparm3 FUN_0007a9fc(int *param_1,undefined4 param_2)

{
  int iVar1;
  
  if (*(int *)((int)param_1 + *(int *)(*param_1 + 4) + 0x10) == 0) {
    iVar1 = (**(code **)(*(int *)(*(int *)((int)param_1 + *(int *)(*param_1 + 4) + 4) + 0x28) + 0x20
                        ))();
    if (iVar1 == -1) {
      FUN_0007a9c0();
    }
  }
  return CONCAT44(param_2,param_1);
}

