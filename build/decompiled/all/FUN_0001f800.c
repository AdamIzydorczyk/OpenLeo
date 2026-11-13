
void __regparm3 FUN_0001f800(int param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = *(uint *)(param_2 + 4);
  if (0x11 < uVar1) {
    if (uVar1 < 0x13) {
      if (*(int *)(param_2 + 0xc) == *(int *)(param_1 + 0xee)) {
        FUN_000749a0();
        FUN_00049580();
        return;
      }
      return;
    }
    if (uVar1 == 0x20) {
      *(undefined4 *)(param_1 + 0x28) = 1;
      return;
    }
  }
  FUN_00054560();
  return;
}

