
void __regparm3 FUN_00013c14(undefined4 param_1,ushort param_2)

{
  char cVar2;
  byte bVar1;
  uint uVar3;
  uint uVar4;
  
  uVar3 = *(int *)(DAT_000afb74 + 0x53) >> 0x1a & 0x1f;
  uVar4 = (int)(uint)*(byte *)(DAT_000afb74 + 0x52) >> 4 & 3;
  if (uVar4 == 0) {
    param_2 = (ushort)(byte)(&DAT_000a734c)[uVar3];
  }
  else if (uVar4 < 2) {
    cVar2 = (char)uVar3;
    bVar1 = cVar2 * '\b';
    if (*(int *)(DAT_000afb74 + 0x53) >> 0x18 < 0) {
      bVar1 = cVar2 * -8 - 1;
    }
    param_2 = (ushort)bVar1;
  }
  else if (uVar4 == 2) {
    param_2 = 0xff;
  }
  cVar2 = (char)((ushort)(param_2 * *(byte *)(DAT_000afb74 + 0x58)) >> 6);
  if (*(int *)(DAT_000afb74 + 0x53) >> 0x18 < 0) {
    *(char *)(DAT_000afb74 + 0x33) = *(char *)(DAT_000afb74 + 0x43) - cVar2;
    if (*(int *)(DAT_000afb74 + 0x30) >> 0x18 < 0) {
      *(undefined1 *)(DAT_000afb74 + 0x33) = 0;
    }
  }
  else {
    *(char *)(DAT_000afb74 + 0x33) = cVar2 + *(char *)(DAT_000afb74 + 0x43);
    if (0x40 < *(int *)(DAT_000afb74 + 0x30) >> 0x18) {
      *(undefined1 *)(DAT_000afb74 + 0x33) = 0x40;
    }
  }
  if (DAT_000aefec != 0) {
    *(char *)(DAT_000afb74 + 0x56) = *(char *)(DAT_000afb74 + 0x56) + *(char *)(DAT_000afb74 + 0x57)
    ;
  }
  return;
}

