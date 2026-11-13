
void FUN_00015484(void)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  
  iVar1 = (uint)DAT_000afb8a - (uint)DAT_000afb8c;
  if ((((int)(uint)*(byte *)(DAT_000afb84 + (uint)DAT_000afb8e) >> 5) + 1U < 8) &&
     ((ushort)iVar1 == (*(byte *)(DAT_000afb84 + (uint)DAT_000afb8e) & 0x1f))) {
    iVar2 = FUN_00015454();
    iVar1 = extraout_ECX;
    if (iVar2 != 0) {
      *(char *)((uint)DAT_000afb8e + DAT_000afb84) =
           *(char *)((uint)DAT_000afb8e + DAT_000afb84) + ' ';
      goto LAB_00015543;
    }
  }
  *(char *)((uint)DAT_000afb8c + DAT_000afb84) = (char)iVar1;
  DAT_000afb8e = DAT_000afb8c;
  DAT_000afb8c = DAT_000afb8a;
LAB_00015543:
  DAT_000afb8a = DAT_000afb8c + 1;
  return;
}

