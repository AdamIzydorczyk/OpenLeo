
void FUN_00027030(void)

{
  int iVar1;
  
  iVar1 = FUN_000500b0();
  *(undefined2 *)(iVar1 + 0x12a) = 0;
  *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009a54c;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009a544;
  *(undefined4 *)(iVar1 + 0xf6) = 0x5dc;
  return;
}

