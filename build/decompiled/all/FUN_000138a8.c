
void FUN_000138a8(void)

{
  if ((*(byte *)(DAT_000aefe4 + 0x12d) & 1) == 0) {
    FUN_00013840();
    return;
  }
  if ((*(byte *)(DAT_000aefe4 + 0x12d) & 2) == 0) {
    FUN_000137bc();
    return;
  }
  FUN_00013794();
  return;
}

