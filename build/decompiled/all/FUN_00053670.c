
void __regparm3 FUN_00053670(int param_1,undefined4 param_2)

{
  if ((*(uint *)(*(int *)(*(int *)(param_1 + 0x50) + *(int *)(param_1 + 0x5c) * 4) + 0x12) & 0x800)
      != 0) {
    *(int *)(param_1 + 0x6d) = -*(int *)(param_1 + 0x6d);
    (**(code **)(*(int *)(param_1 + 8) + 0x54))(param_2);
  }
  return;
}

