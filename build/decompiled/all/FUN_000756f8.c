
void __regparm3 FUN_000756f8(int param_1)

{
  int iVar1;
  int iVar2;
  int extraout_ECX;
  
  iVar1 = *(int *)(param_1 + 0xc);
  iVar2 = *(int *)(*(int *)(param_1 + 8) + 8);
  while (iVar2 != iVar1) {
    FUN_0007569a();
    iVar2 = extraout_ECX;
  }
  return;
}

