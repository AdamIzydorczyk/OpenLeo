
void __regparm3 FUN_00013b68(undefined4 param_1,ushort param_2)

{
  char cVar1;
  uint uVar2;
  ushort uVar3;
  byte bVar4;
  
  bVar4 = *(byte *)(DAT_000afb74 + 0x52) & 3;
  uVar2 = *(int *)(DAT_000afb74 + 0x50) >> 0x1a & 0x1f;
  if (bVar4 == 0) {
    param_2 = (ushort)(byte)(&DAT_000a734c)[uVar2];
  }
  else if (bVar4 < 2) {
    cVar1 = (char)uVar2;
    bVar4 = cVar1 * '\b';
    if (*(int *)(DAT_000afb74 + 0x50) >> 0x18 < 0) {
      bVar4 = cVar1 * -8 - 1;
    }
    param_2 = (ushort)bVar4;
  }
  else if (bVar4 == 2) {
    param_2 = 0xff;
  }
  uVar3 = (ushort)(param_2 * *(byte *)(DAT_000afb74 + 0x55)) >> 7;
  if (*(int *)(DAT_000afb74 + 0x50) >> 0x18 < 0) {
    *(ushort *)(DAT_000afb74 + 0x34) = *(short *)(DAT_000afb74 + 0x44) + uVar3 * -4;
  }
  else {
    *(ushort *)(DAT_000afb74 + 0x34) = uVar3 * 4 + *(short *)(DAT_000afb74 + 0x44);
  }
  if (DAT_000aefec != 0) {
    *(char *)(DAT_000afb74 + 0x53) = *(char *)(DAT_000afb74 + 0x53) + *(char *)(DAT_000afb74 + 0x54)
    ;
  }
  return;
}

