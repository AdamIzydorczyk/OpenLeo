
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm3 FUN_00013e38(byte param_1)

{
  uint uVar1;
  char cVar2;
  
  uVar1 = ram0x000a732e;
  if (param_1 != 0) {
    DAT_000afb70 = param_1;
  }
  if (DAT_000aefec != 0) {
    cVar2 = (char)((int)(uint)DAT_000afb70 >> 4);
    if ((int)(uint)DAT_000afb70 >> 4 == 0) {
      cVar2 = -(DAT_000afb70 & 0xf);
    }
    cVar2 = DAT_000a7331 + cVar2;
    ram0x000a732e = CONCAT13(cVar2,ram0x000a732e);
    if (cVar2 < '\0') {
      ram0x000a732e = uVar1 & 0xffffff;
      return;
    }
    if ('@' < cVar2) {
      ram0x000a732e = CONCAT13(0x40,ram0x000a732e);
    }
  }
  return;
}

