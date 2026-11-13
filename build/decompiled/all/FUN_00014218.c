
void FUN_00014218(void)

{
  uint uVar1;
  
  if (DAT_000aefec == 0) {
    *(undefined2 *)(DAT_000afb74 + 0x44) = *(undefined2 *)(DAT_000afb74 + 0x34);
    return;
  }
  uVar1 = (uint)*(ushort *)(DAT_000afb74 + 0x34) - (uint)*(ushort *)(DAT_000afb74 + 0x46);
  if ((uVar1 == 0) ||
     ((int)((uVar1 ^ (int)uVar1 >> 0x1f) - ((int)uVar1 >> 0x1f)) <
      (int)(uint)*(ushort *)(DAT_000afb74 + 0x4a))) {
    *(undefined2 *)(DAT_000afb74 + 0x34) = *(undefined2 *)(DAT_000afb74 + 0x46);
  }
  else if ((int)uVar1 < 1) {
    *(short *)(DAT_000afb74 + 0x34) =
         *(short *)(DAT_000afb74 + 0x34) + *(short *)(DAT_000afb74 + 0x4a);
  }
  else {
    *(short *)(DAT_000afb74 + 0x34) =
         *(short *)(DAT_000afb74 + 0x34) - *(short *)(DAT_000afb74 + 0x4a);
  }
  *(undefined2 *)(DAT_000afb74 + 0x44) = *(undefined2 *)(DAT_000afb74 + 0x34);
  return;
}

