
bool __regparm3 FUN_000587d0(int *param_1)

{
  int iVar1;
  int *extraout_ECX;
  
  iVar1 = (**(code **)(*param_1 + 0xc))();
  if (iVar1 != 0) {
    (**(code **)(*extraout_ECX + 0x14))(iVar1);
  }
  return iVar1 != 0;
}

