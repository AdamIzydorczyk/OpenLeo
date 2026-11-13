
void FUN_00062e40(void)

{
  int iVar1;
  
  iVar1 = FUN_0003ff70();
  *(undefined4 *)(iVar1 + 0x120) = 0;
  *(undefined4 *)(iVar1 + 0x124) = 0;
  *(undefined ***)(iVar1 + 8) = &PTR_LAB_00097bc0;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_00097bb8;
  *(undefined ***)(iVar1 + 8) = &PTR_FUN_000a348c;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_000a3484;
  return;
}

