
void FUN_00021750(void)

{
  int iVar1;
  undefined2 extraout_CX;
  undefined4 in_ECX;
  
  iVar1 = FUN_00021fd0(in_ECX);
  *(undefined ***)(iVar1 + 0x10) = &PTR_FUN_00097644;
  *(undefined2 *)(iVar1 + 0x14) = extraout_CX;
  return;
}

