
void __regparm3 FUN_0003df90(int param_1)

{
  int iVar1;
  byte in_stack_ffffffe0;
  
  FUN_000749a0();
  FUN_00049580(in_stack_ffffffe0 | 0x20);
  if (((*(int *)(param_1 + 0x11a) != 0) || (*(int *)(param_1 + 0x11e) != 0)) &&
     (iVar1 = (**(code **)(*(int *)(*(int *)(param_1 + 0x112) + 8) + 0x3c))(), iVar1 != 0)) {
    (**(code **)(*(int *)(*(int *)(param_1 + 0x112) + 8) + 0x70))();
    FUN_00052b10();
    FUN_00052b10();
  }
  *(undefined4 *)(param_1 + 0x112) = 0;
  return;
}

