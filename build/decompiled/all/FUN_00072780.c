
void FUN_00072780(void)

{
  int iVar1;
  
  iVar1 = FUN_0003ff70();
  *(undefined4 *)(iVar1 + 300) = 0;
  *(undefined4 *)(iVar1 + 0x130) = 0;
  *(undefined ***)(iVar1 + 8) = &PTR_FUN_0009f380;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009f378;
  *(undefined ***)(iVar1 + 8) = &PTR_LAB_000a6520;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_000a6518;
  return;
}

