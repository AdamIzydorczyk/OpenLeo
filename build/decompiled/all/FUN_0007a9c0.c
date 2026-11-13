
void __regparm3 FUN_0007a9c0(int param_1,uint param_2)

{
  *(uint *)(param_1 + 0x10) = *(uint *)(param_1 + 0x10) | param_2;
  if ((*(uint *)(param_1 + 0x10) & *(uint *)(param_1 + 0x14)) != 0) {
    FUN_0007fb7c();
    FUN_00075b10();
  }
  return;
}

