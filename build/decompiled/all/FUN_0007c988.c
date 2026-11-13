
void __regparm3 FUN_0007c988(int param_1)

{
  ushort uVar1;
  int unaff_EBP;
  
  uVar1 = *(ushort *)(unaff_EBP + 4) & 0xb8ff;
  *(ushort *)(unaff_EBP + 4) = CONCAT11((byte)(uVar1 >> 8) | (&DAT_0007c433)[param_1],(char)uVar1);
  FUN_0007c794();
  return;
}

