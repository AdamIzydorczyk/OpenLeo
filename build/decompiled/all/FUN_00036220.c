
void FUN_00036220(void)

{
  int iVar1;
  
  iVar1 = FUN_000500b0();
  *(undefined4 *)(iVar1 + 0x12a) = 0;
  *(undefined4 *)(iVar1 + 0x13e) = 0xffffffff;
  *(undefined4 *)(iVar1 + 0x142) = 0xffffffff;
  *(undefined4 *)(iVar1 + 0x146) = 0;
  *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009f170;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009f168;
  *(undefined4 *)(iVar1 + 0xf6) = 0x8fc;
  return;
}

