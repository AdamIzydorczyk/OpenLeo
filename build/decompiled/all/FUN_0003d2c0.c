
void __regparm3 FUN_0003d2c0(int param_1)

{
  if ((*(byte *)(param_1 + 0x67) & 0x20) != 0) {
    (**(code **)(*(int *)(param_1 + 8) + 0xc))();
    FUN_00052b10(*(undefined4 *)(DAT_000cef40 + 0x394));
  }
  if ((*(byte *)(param_1 + 0x67) & 0x40) != 0) {
    FUN_00044920();
  }
  FUN_00040030();
  return;
}

