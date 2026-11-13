
void __regparm3 FUN_00075302(uint param_1)

{
  uint uVar1;
  uint extraout_ECX;
  
  if (param_1 != 0) {
    if (((DAT_000cf530 == 0) || (param_1 < DAT_000cf530)) ||
       (*(uint *)(DAT_000cf530 + 8) <= param_1)) {
      for (uVar1 = DAT_000a9b58;
          (*(uint *)(uVar1 + 8) != 0 && ((param_1 < uVar1 || (*(uint *)(uVar1 + 8) <= param_1))));
          uVar1 = *(uint *)(uVar1 + 8)) {
      }
    }
    FUN_00078d88();
    if ((extraout_ECX < DAT_000a9b5c) && (DAT_000a9b60 < *(uint *)(extraout_ECX + 0x14))) {
      DAT_000a9b60 = *(uint *)(extraout_ECX + 0x14);
    }
    DAT_000cf9c0 = 0;
    DAT_000cf530 = extraout_ECX;
  }
  return;
}

