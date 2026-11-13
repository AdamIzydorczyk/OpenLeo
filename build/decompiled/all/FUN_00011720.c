
void __regparm3 FUN_00011720(byte param_1,undefined4 param_2)

{
  uint uVar1;
  
  DAT_000ae63c = '\0';
  DAT_000ae63d = 0;
  for (uVar1 = (uint)(0x7a12 / (ulonglong)(longlong)(int)(uint)param_1); 0xff < uVar1;
      uVar1 = uVar1 >> 1) {
    DAT_000ae63c = DAT_000ae63c + '\x01';
  }
  FUN_00011610(param_2);
  return;
}

