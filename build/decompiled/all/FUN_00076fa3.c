
undefined8 __regparm3 FUN_00076fa3(byte *param_1,undefined4 param_2)

{
  byte bVar1;
  byte bVar2;
  int iVar3;
  
  while (((&DAT_000a68b4)[(byte)(*param_1 + 1)] & 2) != 0) {
    param_1 = param_1 + 1;
  }
  bVar1 = *param_1;
  if ((bVar1 == 0x2b) || (bVar1 == 0x2d)) {
    param_1 = param_1 + 1;
  }
  iVar3 = 0;
  while (((&DAT_000a68b4)[(byte)(*param_1 + 1)] & 0x20) != 0) {
    bVar2 = *param_1;
    param_1 = param_1 + 1;
    iVar3 = iVar3 * 10 + (uint)bVar2 + -0x30;
  }
  if (bVar1 == 0x2d) {
    iVar3 = -iVar3;
  }
  return CONCAT44(param_2,iVar3);
}

