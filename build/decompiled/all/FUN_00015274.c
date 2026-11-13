
byte * __regparm3 FUN_00015274(byte *param_1,uint param_2)

{
  byte bVar1;
  uint uVar2;
  
  while( true ) {
    bVar1 = *param_1;
    if (bVar1 == 0) {
      return (byte *)0x0;
    }
    uVar2 = ((int)(uint)bVar1 >> 5) + 1;
    if ((param_2 & 0xffff) < uVar2) break;
    param_2 = param_2 - CONCAT11((byte)((param_2 & 0xffff) >> 8) ^ (byte)(param_2 >> 8),(char)uVar2)
    ;
    param_1 = param_1 + (bVar1 & 0x1f);
  }
  return param_1;
}

