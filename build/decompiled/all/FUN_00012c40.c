
void __regparm3 FUN_00012c40(byte *param_1,undefined4 param_2,undefined4 param_3)

{
  char cVar1;
  uint uVar2;
  uint3 uVar3;
  uint extraout_EDX;
  ushort uVar4;
  
  uVar4 = (ushort)param_1[1] + (*param_1 & 0xff0f) * 0x100;
  uVar2 = (uint)(byte)(*param_1 & 0x10 | (byte)((int)(uint)param_1[2] >> 4)) << 8;
  if (uVar4 != 0) {
    while( true ) {
      uVar3 = (uint3)(uVar2 >> 8);
      if ((0x3b < (uVar2 & 0xff)) || (*(ushort *)(&DAT_000a7272 + (uVar2 & 0xff) * 2) <= uVar4))
      break;
      uVar2 = CONCAT31(uVar3,(char)uVar2 + '\x01');
    }
    cVar1 = (char)uVar2 + '\x01';
    uVar2 = CONCAT31(uVar3,cVar1);
    if (cVar1 == '=') {
      uVar2 = (uint)uVar3 << 8;
    }
  }
  if (uVar2 >> 8 != 0) {
    FUN_000153fc(param_2,param_3);
    uVar2 = extraout_EDX;
  }
  if ((uVar2 & 0xff) != 0) {
    FUN_00015414();
  }
  FUN_0001542c();
  return;
}

