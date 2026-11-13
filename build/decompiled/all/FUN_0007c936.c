
void __regparm3 FUN_0007c936(int param_1)

{
  ushort uVar1;
  int unaff_EBP;
  int unaff_EDI;
  
  uVar1 = *(ushort *)(unaff_EBP + 4) & 0xb8ff;
  *(ushort *)(unaff_EBP + 4) = CONCAT11((byte)(uVar1 >> 8) | (&DAT_0007c433)[param_1],(char)uVar1);
  *(ushort *)(unaff_EBP + 8) = *(ushort *)(unaff_EBP + 8) & *(ushort *)(&DAT_0007c3e2 + unaff_EDI);
  *(ushort *)(unaff_EBP + 8) = *(ushort *)(unaff_EBP + 8) | *(ushort *)(&DAT_0007c3ea + unaff_EDI);
  uVar1 = *(ushort *)(&DAT_0007c396 + *(ushort *)(&DAT_0007c392 + unaff_EDI));
  *(ushort *)(unaff_EBP + 4) = *(ushort *)(unaff_EBP + 4) & 0xc7ff;
  *(ushort *)(unaff_EBP + 4) = *(ushort *)(unaff_EBP + 4) | uVar1;
  FUN_0007c794();
  return;
}

