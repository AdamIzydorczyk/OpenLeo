
void FUN_00049610(void)

{
  int iVar1;
  undefined4 extraout_ECX;
  undefined4 unaff_EBX;
  
  iVar1 = FUN_00047d50();
  *(undefined4 *)(iVar1 + 0xd) = unaff_EBX;
  *(undefined4 *)(iVar1 + 0x11) = extraout_ECX;
  return;
}

