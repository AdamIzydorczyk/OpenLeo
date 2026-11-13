
void __regparm3 FUN_00013dd0(char param_1)

{
  int iVar1;
  
  if (param_1 != '\0') {
    *(char *)(DAT_000afb74 + 0x4e) = param_1;
  }
  if (DAT_000aefec != 0) {
    iVar1 = (int)(uint)*(byte *)(DAT_000afb74 + 0x4e) >> 4;
    if (iVar1 == 0) {
      *(char *)(DAT_000afb74 + 0x43) =
           *(char *)(DAT_000afb74 + 0x43) - (*(byte *)(DAT_000afb74 + 0x4e) & 0xf);
    }
    else {
      *(char *)(DAT_000afb74 + 0x43) = *(char *)(DAT_000afb74 + 0x43) + (char)iVar1;
    }
    iVar1 = *(int *)(DAT_000afb74 + 0x40) >> 0x18;
    if (iVar1 < 0) {
      *(undefined1 *)(DAT_000afb74 + 0x43) = 0;
      return;
    }
    if (0x40 < iVar1) {
      *(undefined1 *)(DAT_000afb74 + 0x43) = 0x40;
    }
  }
  return;
}

