
void __regparm3 FUN_0001a060(undefined4 param_1,undefined4 param_2)

{
  byte bVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  ushort extraout_DX;
  ushort extraout_DX_00;
  
  FUN_00019470(param_2);
  out(DAT_000c40ce,DAT_000c40dd);
  out(DAT_000c40d0,DAT_000c40de);
  out(DAT_000c4072,0);
  out(DAT_000c40d2,(undefined1)DAT_000c40fa);
  out(DAT_000c40d2,(char)((ushort)DAT_000c40fa >> 8));
  out(DAT_000c40d4,DAT_000c40f8);
  out(DAT_000c4072,0);
  DAT_000c40a4 = 0;
  DAT_000c40a0 = 0;
  DAT_000c409e = 1;
  if ((DAT_000c40b0 & 1) == 0) {
    out(DAT_000c4074,(char)DAT_000c40e0 + -1);
    uVar3 = (uint)DAT_000c40e0;
  }
  else {
    out(DAT_000c4074,(char)((int)(uint)DAT_000c40e0 >> 1) + -1);
    uVar3 = (int)(uint)DAT_000c40e0 >> 1;
  }
  out(DAT_000c4074,(char)(uVar3 - 1 >> 8));
  DAT_000c4078 = (uint)DAT_000c40e0;
  out(DAT_000c40ce,DAT_000c40dc);
  if ((DAT_000c40ac & 1) == 0) {
    FUN_00019770();
    FUN_00019680();
    if ((DAT_000c40b0 & 2) == 0) {
      FUN_0001a040();
    }
    else {
      bVar1 = FUN_0001a040();
      if (bVar1 < 0xea) {
        FUN_0001a040();
      }
    }
    FUN_00019680();
    if ((DAT_000c40ac & 4) == 0) {
      FUN_00019680();
      FUN_00019680();
    }
    else {
      FUN_00019680();
      FUN_00019680();
      FUN_00019680();
      if ((DAT_000c40b0 & 2) == 0) {
        out(DAT_000c40c0,0xe);
        bVar1 = in(DAT_000c40c2);
        out(DAT_000c40c2,bVar1 & 0xfd);
      }
      else {
        out(DAT_000c40c0,0xe);
        bVar1 = in(DAT_000c40c2);
        out(DAT_000c40c2,bVar1 | 2);
      }
    }
  }
  else {
    FUN_00019680();
    FUN_00019680();
    FUN_00019680();
    FUN_00019680();
    FUN_00019680();
    if ((DAT_000c40b0 & 1) == 0) {
      FUN_00019680();
    }
    else {
      FUN_00019680();
    }
  }
  FUN_00019680();
  iVar4 = FUN_000193e0();
  if (iVar4 == 0) {
    do {
      uVar2 = FUN_0001b230();
    } while (uVar2 < extraout_DX);
    do {
      uVar2 = FUN_0001b230();
    } while (extraout_DX_00 < uVar2);
    FUN_0001a840();
    FUN_0001a980();
  }
  return;
}

