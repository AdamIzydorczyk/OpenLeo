
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint __regparm3 FUN_0001b7b0(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  int unaff_EBX;
  
  if ((DAT_000cde1c != DAT_000cde18) || (uVar1 = DAT_000cde1c, unaff_EBX != 0)) {
    *(undefined4 *)(&DAT_000c41d4 + DAT_000cde1c * 4) = param_2;
    DAT_000cde1c = DAT_000cde1c + 1;
    uVar1 = DAT_000cde1c / 10000;
    DAT_000cde1c = DAT_000cde1c % 10000;
  }
  _DAT_000cde20 = 0;
  return uVar1;
}

