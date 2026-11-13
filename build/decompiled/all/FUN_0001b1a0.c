
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0001b1a0(void)

{
  int iVar1;
  
  if (DAT_000cde28 != 0) {
    iVar1 = DAT_000cde1c - DAT_000cde18;
    if (iVar1 < 0) {
      iVar1 = -iVar1;
    }
    if ((-1 < iVar1 + -2) && ((_DAT_000cde20 & 1) == 0)) {
      _DAT_000cde20 = 1;
      DAT_000cde30 = *(int *)(&DAT_000c41d4 + DAT_000cde18 * 4) - DAT_000cde34;
      DAT_000cde18 = DAT_000cde18 + 1;
      if (DAT_000cde18 == 10000) {
        DAT_000cde18 = 0;
      }
      FUN_0001b0d9();
    }
  }
  _DAT_000cde20 = 0;
  return;
}

