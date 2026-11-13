
ulonglong __regparm3 FUN_000116a0(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  if (param_1 == 1) {
    DAT_000ade97 = DAT_000ade97 & 0xfb;
    DAT_000ade98 = DAT_000ade98 & 0xfe;
  }
  else {
    DAT_000ade97 = DAT_000ade97 & 0xf7;
    DAT_000ade98 = DAT_000ade98 & 0xfd;
  }
  uVar1 = FUN_000105dc();
  out(DAT_000ade94 + 8,4);
  out(DAT_000ade94 + 9,DAT_000ade98 | 0x80);
  return CONCAT44(param_2,CONCAT31((int3)((uint)uVar1 >> 8),DAT_000ade98)) | 0x80;
}

