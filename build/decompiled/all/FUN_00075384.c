
void __regparm3 FUN_00075384(int param_1,undefined4 param_2)

{
  if (*(int *)(param_1 + 8) != 0) {
    *(undefined1 *)(*(int *)(param_1 + 8) + 0x1c) = 3;
  }
  *(byte *)(param_1 + 4) = *(byte *)(param_1 + 4) | 1;
  thunk_FUN_0007c1b4(param_2);
  return;
}

