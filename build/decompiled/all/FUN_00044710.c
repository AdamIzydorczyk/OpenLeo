
void __regparm3 FUN_00044710(int param_1)

{
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  
  (**(code **)(*(int *)(**(int **)(*(int *)(param_1 + 0x84) + 8) + 0x33) + 0x10))();
  (**(code **)(*(int *)(*(int *)((*(int *)(extraout_ECX + 0x68) / 10) * 4 +
                                *(int *)(*(int *)(extraout_ECX + 0x80) + 8)) + 0x33) + 0x10))();
  (**(code **)(*(int *)(*(int *)(*(int *)(*(int *)(extraout_ECX_00 + 0x80) + 8) +
                                (*(int *)(extraout_ECX_00 + 0x68) % 10) * 4) + 0x33) + 0x10))();
  *(int *)(extraout_ECX_01 + 0x58) = *(int *)(extraout_ECX_01 + 0x58) + -1;
  return;
}

