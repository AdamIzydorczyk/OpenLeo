
void __regparm3 FUN_00013d44(char param_1)

{
  char cVar1;
  ushort uVar2;
  int iVar3;
  
  if (param_1 != '\0') {
    *(char *)(DAT_000afb74 + 0x4e) = param_1;
  }
  uVar2 = CONCAT11(*(undefined1 *)(DAT_000afb74 + 0x4e),*(undefined1 *)(DAT_000afb74 + 0x4e)) &
          0xfff;
  iVar3 = (int)(uint)(byte)uVar2 >> 4;
  cVar1 = (char)(uVar2 >> 8);
  if (iVar3 == 0) {
LAB_00013d72:
    *(char *)(DAT_000afb74 + 0x43) = *(char *)(DAT_000afb74 + 0x43) - cVar1;
  }
  else {
    if (cVar1 != '\0') {
      if (iVar3 == 0xf) {
        if (DAT_000aefec != 0) goto LAB_00013da2;
        goto LAB_00013d72;
      }
      if ((cVar1 != '\x0f') || (DAT_000aefec != 0)) goto LAB_00013da2;
    }
    *(char *)(DAT_000afb74 + 0x43) = *(char *)(DAT_000afb74 + 0x43) + (char)iVar3;
  }
LAB_00013da2:
  if (*(int *)(DAT_000afb74 + 0x40) >> 0x18 < 0) {
    *(undefined1 *)(DAT_000afb74 + 0x43) = 0;
  }
  if (0x40 < *(int *)(DAT_000afb74 + 0x40) >> 0x18) {
    *(undefined1 *)(DAT_000afb74 + 0x43) = 0x40;
  }
  return;
}

