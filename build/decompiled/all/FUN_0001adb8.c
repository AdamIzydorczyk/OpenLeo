
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0001adb8(void)

{
  byte bVar1;
  uint uVar2;
  uint extraout_EDX;
  uint extraout_EDX_00;
  uint extraout_EDX_01;
  uint extraout_EDX_02;
  uint extraout_EDX_03;
  byte *unaff_ESI;
  byte *pbVar3;
  
  _DAT_000a8950 = DAT_000c41c4;
  do {
    bVar1 = *unaff_ESI;
    pbVar3 = unaff_ESI + 1;
    uVar2 = (uint)(bVar1 & 7);
    if ((bVar1 & 0x40) != 0) {
      uVar2 = (uint)(CONCAT11(bVar1,*pbVar3) & 0x7ff);
      pbVar3 = unaff_ESI + 2;
    }
    bVar1 = bVar1 & 0xb8;
    if (bVar1 == 0) {
      FUN_0001a998();
      uVar2 = extraout_EDX;
    }
    else if (bVar1 == 8) {
      FUN_0001a9fe();
      uVar2 = extraout_EDX_00;
    }
    else if (bVar1 == 0x10) {
      FUN_0001ab9b();
      uVar2 = extraout_EDX_01;
    }
    else if (bVar1 == 0x18) {
      FUN_0001ac29();
      uVar2 = extraout_EDX_02;
    }
    else if (bVar1 == 0x20) {
      FUN_0001ab32();
      uVar2 = extraout_EDX_03;
    }
    else {
      if (bVar1 == 0x88) {
        DAT_000a8970 = uVar2;
        return;
      }
      if (bVar1 == 0x90) {
        DAT_000a893c = DAT_000a893c + uVar2;
        DAT_000a8940 = DAT_000a8940 + uVar2;
        DAT_000a8944 = DAT_000a8944 + uVar2;
        DAT_000a8948 = DAT_000a8948 + uVar2;
      }
    }
    iRam00018950 = iRam00018950 - uVar2;
    unaff_ESI = pbVar3;
  } while (iRam00018950 != 0);
  return;
}

