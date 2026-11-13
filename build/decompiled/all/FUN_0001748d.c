
undefined4 __regparm3 FUN_0001748d(uint param_1,uint param_2)

{
  byte bVar2;
  uint uVar1;
  uint uVar3;
  int unaff_EBX;
  uint uVar4;
  
  if (param_2 < param_1) {
    param_1 = param_1 - param_2;
  }
  else {
    param_1 = param_2 - param_1;
  }
  param_1 = (uint)(unaff_EBX * 1000) / param_1;
  uVar4 = 4;
  uVar3 = 0;
  uVar1 = *(uint *)((uint)DAT_000a8863 * 4 + 0xa84d8);
  do {
    if (param_1 < uVar1) {
      uVar4 = uVar3;
      break;
    }
    uVar3 = uVar3 + 1;
    uVar1 = uVar1 << 3;
  } while (uVar3 < 3);
  if (uVar4 == 4) {
    uVar4 = 3;
    param_1 = 1;
  }
  else {
    param_1 = (uVar1 + (uVar1 >> 1)) / param_1;
  }
  uVar1 = CONCAT31((int3)(CONCAT22((short)(param_1 >> 0x10),CONCAT11((char)uVar4,(char)param_1)) >>
                         8),(char)param_1) & 0xffffff3f;
  bVar2 = (char)(uVar1 >> 8) << 6;
  return CONCAT31((int3)(CONCAT22((short)(uVar1 >> 0x10),CONCAT11(bVar2,(byte)uVar1)) >> 8),
                  (byte)uVar1 | bVar2);
}

