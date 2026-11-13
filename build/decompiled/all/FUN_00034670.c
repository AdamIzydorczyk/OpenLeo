
void __regparm3 FUN_00034670(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = (**(code **)(*(int *)(param_2 + 8) + 0x3c))(param_3);
  if (iVar1 != 0) {
    *(undefined4 *)(param_1 + 0x132) = 1;
  }
  FUN_0003df50();
  return;
}

