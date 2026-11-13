
void __regparm3 FUN_000142a0(char param_1)

{
  undefined2 uVar1;
  
  if (param_1 == '\0') {
    return;
  }
  uVar1 = FUN_000138a8();
  *(undefined2 *)(DAT_000afb74 + 0x34) = uVar1;
  *(undefined2 *)(DAT_000afb74 + 0x38) = 1;
  return;
}

