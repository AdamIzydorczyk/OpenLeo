
int __regparm3 FUN_00077e36(byte *param_1,undefined1 *param_2)

{
  byte bVar2;
  uint uVar1;
  int unaff_EBX;
  
  while( true ) {
    if (unaff_EBX == 0) {
      return 0;
    }
    bVar2 = *param_1;
    uVar1 = (uint)CONCAT11(*param_2,bVar2);
    if ((0x40 < bVar2) && (bVar2 < 0x5b)) {
      uVar1 = (uint)CONCAT11(*param_2,bVar2 + 0x20);
    }
    bVar2 = (byte)(uVar1 >> 8);
    if ((0x40 < bVar2) && (bVar2 < 0x5b)) {
      uVar1 = (uint)CONCAT11(bVar2 + 0x20,(char)uVar1);
    }
    bVar2 = (byte)(uVar1 >> 8);
    if ((byte)uVar1 != bVar2) break;
    if (bVar2 == 0) {
      return 0;
    }
    param_1 = param_1 + 1;
    param_2 = param_2 + 1;
    unaff_EBX = unaff_EBX + -1;
  }
  return (uVar1 & 0xff) - (uint)bVar2;
}

