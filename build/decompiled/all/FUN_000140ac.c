
void __regparm3 FUN_000140ac(byte param_1)

{
  byte bVar1;
  char cVar2;
  
  cVar2 = (char)((int)(uint)param_1 >> 4);
  if ((param_1 & 0xf) != 0) {
    *(char *)(DAT_000afb74 + 0x50) = cVar2;
    *(byte *)(DAT_000afb74 + 0x4f) = param_1 & 0xf;
  }
  if (cVar2 != '\0') {
    *(char *)(DAT_000afb74 + 0x50) = cVar2;
  }
  if (*(char *)(DAT_000afb74 + 0x4f) == '\0') {
    return;
  }
  if (*(int *)(DAT_000afb74 + 0x3d) >> 0x18 != 0) goto LAB_000141ce;
  *(undefined1 *)(DAT_000afb74 + 0x29) = 1;
  *(undefined1 *)(DAT_000afb74 + 0x40) = *(undefined1 *)(DAT_000afb74 + 0x4f);
  if (DAT_000aefec == 0) goto LAB_000141ce;
  bVar1 = *(byte *)(DAT_000afb74 + 0x50);
  if (bVar1 < 7) {
    if (bVar1 != 0) {
      if (5 < bVar1) {
        cVar2 = (char)(((*(int *)(DAT_000afb74 + 0x40) >> 0x18) * 2) / 3);
        goto LAB_000141a4;
      }
      *(char *)(DAT_000afb74 + 0x43) =
           *(char *)(DAT_000afb74 + 0x43) - ('\x01' << (bVar1 - 1 & 0x1f));
    }
  }
  else {
    if (bVar1 < 8) {
      cVar2 = (char)((int)*(undefined4 *)(DAT_000afb74 + 0x40) >> 0x19);
    }
    else {
      if (bVar1 < 0xe) {
        if (8 < bVar1) {
          *(char *)(DAT_000afb74 + 0x43) =
               *(char *)(DAT_000afb74 + 0x43) + ('\x01' << (bVar1 - 9 & 0x1f));
        }
        goto LAB_000141a7;
      }
      if (bVar1 < 0xf) {
        cVar2 = (char)(((*(int *)(DAT_000afb74 + 0x40) >> 0x18) * 3) / 2);
      }
      else {
        if (bVar1 != 0xf) goto LAB_000141a7;
        cVar2 = *(char *)(DAT_000afb74 + 0x43) * '\x02';
      }
    }
LAB_000141a4:
    *(char *)(DAT_000afb74 + 0x43) = cVar2;
  }
LAB_000141a7:
  if (*(int *)(DAT_000afb74 + 0x40) >> 0x18 < 0) {
    *(undefined1 *)(DAT_000afb74 + 0x43) = 0;
  }
  if (0x40 < *(int *)(DAT_000afb74 + 0x40) >> 0x18) {
    *(undefined1 *)(DAT_000afb74 + 0x43) = 0x40;
  }
LAB_000141ce:
  *(char *)(DAT_000afb74 + 0x40) = *(char *)(DAT_000afb74 + 0x40) + -1;
  return;
}

