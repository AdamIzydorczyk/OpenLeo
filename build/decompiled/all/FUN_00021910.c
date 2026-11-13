
void FUN_00021910(void)

{
  int iVar1;
  undefined2 extraout_CX;
  undefined4 in_ECX;
  
  iVar1 = FUN_00022a70(in_ECX);
  *(undefined ***)(iVar1 + 0x10) = &PTR_FUN_0009765c;
  *(undefined2 *)(iVar1 + 0x14) = extraout_CX;
  return;
}

