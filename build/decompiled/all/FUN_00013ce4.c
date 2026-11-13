
void __regparm3 FUN_00013ce4(byte param_1)

{
  if (DAT_000aefec != 0) {
    *(char *)(DAT_000afb74 + 0x43) =
         (char)((uint)*(undefined4 *)(DAT_000afb74 + 0x40) >> 0x18) +
         (char)((int)(uint)param_1 >> 4);
    *(char *)(DAT_000afb74 + 0x43) = *(char *)(DAT_000afb74 + 0x43) - (param_1 & 0xf);
    if (*(int *)(DAT_000afb74 + 0x40) >> 0x18 < 0) {
      *(undefined1 *)(DAT_000afb74 + 0x43) = 0;
    }
    if (0x40 < *(int *)(DAT_000afb74 + 0x40) >> 0x18) {
      *(undefined1 *)(DAT_000afb74 + 0x43) = 0x40;
    }
  }
  return;
}

