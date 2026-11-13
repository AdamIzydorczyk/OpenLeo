
undefined8 __regparm3 FUN_0001c4d0(undefined4 param_1,undefined4 param_2)

{
  short sVar1;
  short *psVar2;
  uint uVar3;
  byte *pbVar4;
  
  psVar2 = DAT_00096364;
  uVar3 = 0;
  if (*DAT_00096364 != *DAT_00096368) {
    pbVar4 = (byte *)(DAT_0009636c + *DAT_00096364);
    uVar3 = (uint)*pbVar4;
    if (uVar3 == 0) {
      uVar3 = (uint)CONCAT11(1,pbVar4[1]);
    }
    sVar1 = *DAT_00096364;
    *DAT_00096364 = sVar1 + 2;
    if (0x3d < (short)(sVar1 + 2)) {
      *psVar2 = 0x1e;
    }
  }
  return CONCAT44(param_2,uVar3);
}

