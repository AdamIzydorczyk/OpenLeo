
void __regparm3 FUN_00041b90(int param_1)

{
  int iVar1;
  
  if (((*(int *)(param_1 + 0xf2) != 0) && (*(char *)(param_1 + 0x160) == '\0')) &&
     (*(int *)(param_1 + 0x5c) == 1)) {
    if (*(char *)(param_1 + 0x106) == '\x01') {
      iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x70))();
      if (iVar1 == 1) {
        FUN_00053670();
      }
    }
    if (*(char *)(param_1 + 0x106) == '\x02') {
      iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x70))();
      if (iVar1 == -1) {
        FUN_00053670();
      }
    }
  }
  (**(code **)(*(int *)(param_1 + 8) + 0xd8))();
  (**(code **)(*(int *)(param_1 + 8) + 0xcc))();
  if (*(int *)(param_1 + 0xee) != 0) {
    *(int *)(param_1 + 0xee) = *(int *)(param_1 + 0xee) + -1;
  }
  FUN_000550a0();
  return;
}

