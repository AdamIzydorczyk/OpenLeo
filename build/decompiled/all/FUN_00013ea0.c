
void __regparm3 FUN_00013ea0(byte param_1)

{
  char cVar1;
  int iVar2;
  ushort uVar3;
  uint uVar4;
  
  if (param_1 == 0) {
    param_1 = *(byte *)(DAT_000afb74 + 0x32);
  }
  else {
    *(byte *)(DAT_000afb74 + 0x32) = param_1;
  }
  if (DAT_000aefec != 0) {
    cVar1 = (char)((int)(uint)param_1 >> 4);
    uVar3 = CONCAT11(cVar1,param_1) & 0xff0f;
    uVar4 = (uint)uVar3;
    if (cVar1 != '\0') {
      uVar4 = (uint)(byte)(uVar3 >> 8) << 8;
    }
    iVar2 = ((uint)*(byte *)(DAT_000afb74 + 0x31) - (uVar4 & 0xff)) + (uVar4 >> 8);
    if ((short)iVar2 < 0) {
      iVar2 = 0;
    }
    if (0xff < (short)iVar2) {
      iVar2 = 0xff;
    }
    *(char *)(DAT_000afb74 + 0x31) = (char)iVar2;
  }
  return;
}

