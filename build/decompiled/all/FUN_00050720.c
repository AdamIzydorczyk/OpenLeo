
void __regparm3 FUN_00050720(int param_1)

{
  byte *extraout_ECX;
  
  if (*(int *)(param_1 + 0x102) != 0) {
    *(int *)(param_1 + 0x102) = *(int *)(param_1 + 0x102) + -1;
  }
  FUN_00050290();
  if ((*extraout_ECX & 0x10) != 0) {
    if (*(int *)(param_1 + 0xf6) < *(int *)(param_1 + 0xc6)) {
      *extraout_ECX = *extraout_ECX | 0x40;
    }
    *(int *)(param_1 + 0xc2) = *(int *)(param_1 + 0xc2) / 3;
  }
  FUN_00054860();
  return;
}

