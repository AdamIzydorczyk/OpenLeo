
void __regparm3 FUN_00014028(byte param_1)

{
  uint uVar1;
  undefined1 uVar2;
  
  if (param_1 == 0) {
    param_1 = *(byte *)(DAT_000afb74 + 0x4d);
  }
  else {
    *(byte *)(DAT_000afb74 + 0x4d) = param_1;
  }
  if (DAT_000aefec != 0) {
    uVar1 = ((int)(uint)param_1 >> 4) + 1;
    *(byte *)(DAT_000afb74 + 0x4c) =
         *(byte *)(DAT_000afb74 + 0x4c) % (byte)((param_1 & 0xf) + 1 + (char)uVar1);
    if (*(byte *)(DAT_000afb74 + 0x4c) < uVar1) {
      uVar2 = (undefined1)((uint)*(undefined4 *)(DAT_000afb74 + 0x40) >> 0x18);
    }
    else {
      uVar2 = 0;
    }
    *(undefined1 *)(DAT_000afb74 + 0x33) = uVar2;
    *(char *)(DAT_000afb74 + 0x4c) = *(char *)(DAT_000afb74 + 0x4c) + '\x01';
  }
  return;
}

