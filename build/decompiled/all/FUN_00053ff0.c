
int FUN_00053ff0(void)

{
  int iVar1;
  
  iVar1 = FUN_000531f0();
  *(undefined1 *)(iVar1 + 0xb5) = 0;
  *(undefined4 *)(iVar1 + 0xd6) = 0;
  *(undefined4 *)(iVar1 + 0xda) = 0;
  *(undefined ***)(iVar1 + 0xde) = &PTR_LAB_00098278;
  *(undefined ***)(iVar1 + 8) = &PTR_LAB_000a23b8;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_000a23b0;
  return iVar1;
}

