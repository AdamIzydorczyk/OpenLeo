
void __regparm3 FUN_00074a02(int param_1,undefined4 param_2,byte param_3)

{
  int unaff_EBX;
  
  if ((*(byte *)(*(int *)(param_1 + 2) + unaff_EBX + 4) & param_3) == 0) {
    (**(code **)(param_1 + 10))();
  }
  return;
}

