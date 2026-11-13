
void FUN_0001c5b0(void)

{
  for (DAT_000cea84 = DAT_000cf1bc; DAT_000cea84 != 0; DAT_000cea84 = *(int *)(DAT_000cea84 + 4)) {
    if ((*(int *)(DAT_000cea84 + 8) < DAT_000cf1b2) && ((*(uint *)(DAT_000cea84 + 0x10) & 1) == 0))
    {
      if (DAT_000cf1b2 < *(int *)(DAT_000cea84 + 8) + *(int *)(DAT_000cea84 + 0xc)) {
        DAT_000cea88 = 0;
      }
      else {
        DAT_000cea88 = (DAT_000cf1b2 - *(int *)(DAT_000cea84 + 8)) / *(int *)(DAT_000cea84 + 0xc);
        *(int *)(DAT_000cea84 + 8) =
             *(int *)(DAT_000cea84 + 8) + (DAT_000cea88 + 1) * *(int *)(DAT_000cea84 + 0xc);
      }
      *(int *)(DAT_000cea84 + 8) = *(int *)(DAT_000cea84 + 8) + *(int *)(DAT_000cea84 + 0xc);
      *(uint *)(DAT_000cea84 + 0x10) = *(uint *)(DAT_000cea84 + 0x10) | 1;
      (*(code *)**(undefined4 **)(DAT_000cea84 + 0x14))();
      *(uint *)(DAT_000cea84 + 0x10) = *(uint *)(DAT_000cea84 + 0x10) & 0xfffffffe;
    }
  }
  DAT_000cf1b2 = DAT_000cf1b2 + DAT_000cf1b6;
  return;
}

