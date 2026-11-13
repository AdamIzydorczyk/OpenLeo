
void FUN_00032530(void)

{
  int iVar1;
  
  iVar1 = FUN_00052860();
  *(undefined4 *)(iVar1 + 100) = 0;
  *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009d2ac;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009d2a4;
  return;
}

