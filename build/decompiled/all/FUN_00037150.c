
int FUN_00037150(void)

{
  int iVar1;
  int unaff_EBX;
  
  while( true ) {
    if (0x10 < unaff_EBX) {
      return 0x11;
    }
    iVar1 = FUN_00036d60();
    if (iVar1 != 0) break;
    unaff_EBX = unaff_EBX + 1;
  }
  return unaff_EBX;
}

