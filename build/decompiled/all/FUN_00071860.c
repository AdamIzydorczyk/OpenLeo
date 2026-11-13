
void FUN_00071860(void)

{
  int iVar1;
  
  iVar1 = FUN_0003ff70();
  *(undefined4 *)(iVar1 + 0x120) = 0;
  *(undefined ***)(iVar1 + 8) = &PTR_LAB_00097e74;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_00097e6c;
  *(undefined ***)(iVar1 + 8) = &PTR_FUN_000a6198;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_000a6190;
  return;
}

