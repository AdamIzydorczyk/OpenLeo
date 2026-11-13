
void FUN_00031ff0(void)

{
  int iVar1;
  
  iVar1 = FUN_000500b0();
  *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009d338;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009d330;
  *(undefined4 *)(iVar1 + 0xfa) = 0xffffffce;
  *(undefined4 *)(iVar1 + 0x12a) = 0;
  *(undefined4 *)(iVar1 + 0x12e) = 0;
  return;
}

