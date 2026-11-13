
void FUN_00018226(void)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  uint uVar5;
  uint extraout_ECX;
  undefined2 uVar6;
  undefined2 uVar7;
  uint uVar8;
  uint uVar9;
  
  uVar8 = 0;
  uVar9 = 0;
  while( true ) {
    uVar6 = (undefined2)DAT_000a887f;
    out(uVar6,0x8f);
    uVar7 = (undefined2)DAT_000a8887;
    bVar1 = in(uVar7);
    bVar2 = bVar1 & 0x1f;
    if ((bVar1 & 0xc0) == 0xc0) break;
    uVar5 = 1 << bVar2;
    if (((bVar1 & 0x80) == 0) && ((uVar8 & uVar5) == 0)) {
      uVar8 = uVar8 | uVar5;
      out((short)DAT_000a887b,bVar2);
      out(uVar6,0x80);
      bVar3 = in(uVar7);
      out(uVar6,0x8d);
      bVar4 = in(uVar7);
      if (((bVar3 & 8) == 0) && ((bVar4 & 4) == 0)) {
        FUN_00017818();
      }
      (*DAT_000a884e)();
      uVar5 = extraout_ECX;
    }
    if ((bVar1 & 0x40) == 0) {
      if ((uVar9 & uVar5) == 0) {
        out((short)DAT_000a887b,bVar2);
        out((short)DAT_000a887f,0x8d);
        uVar9 = uVar9 | uVar5;
        bVar1 = in((short)DAT_000a8887);
        if ((bVar1 & 8) == 0) {
          FUN_000175cc();
        }
        (*DAT_000a8852)();
      }
    }
  }
  return;
}

