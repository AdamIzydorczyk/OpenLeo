
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0001b0d9(void)

{
  ushort *puVar1;
  byte bVar2;
  int iVar3;
  ushort *unaff_ESI;
  
  while( true ) {
    while( true ) {
      while( true ) {
        puVar1 = (ushort *)((int)unaff_ESI + 1);
        bVar2 = (byte)*unaff_ESI & 0xb8;
        if (bVar2 != 0xb0) break;
        DAT_000c40a0 = (byte *)((int)unaff_ESI + 5);
        DAT_000c40a4 = *(int *)puVar1;
        unaff_ESI = (ushort *)(DAT_000c40a0 + DAT_000c40a4);
      }
      if (bVar2 != 0x98) break;
      unaff_ESI = unaff_ESI + 1;
      DAT_000c41d0._0_1_ = *(byte *)puVar1;
      DAT_000c410c = (byte)DAT_000c41d0 - 1;
    }
    if (bVar2 == 0xb8) break;
    if (bVar2 == 0xa8) {
      unaff_ESI = (ushort *)((int)unaff_ESI + 3);
      DAT_000c410c = *puVar1 - 1;
      DAT_000cde2c._0_2_ = 1;
    }
    else {
      if (bVar2 != 0xa0) {
        _DAT_000a8960 = DAT_000cde3c * DAT_000cde24 - DAT_000c41c4;
        DAT_000a8958 = DAT_000cde3c * DAT_000cde24 * 4;
        DAT_000a895c = DAT_000a8958 - DAT_000c41c4;
        if (DAT_000cde38 == 1) {
          FUN_0001ae78();
        }
        else {
          FUN_0001af71();
        }
        return;
      }
      iVar3 = 0x300;
      out(0x3c8,0);
      unaff_ESI = puVar1;
      while (iVar3 != 0) {
        iVar3 = iVar3 + -1;
        out((byte)*unaff_ESI,0x3c9);
        unaff_ESI = (ushort *)((int)unaff_ESI + 1);
      }
    }
  }
  return;
}

