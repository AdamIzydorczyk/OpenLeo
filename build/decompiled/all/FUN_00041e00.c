
void __regparm3 FUN_00041e00(int param_1)

{
  int extraout_ECX;
  int iVar1;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int iVar2;
  int iVar3;
  
  iVar3 = param_1 + 6;
  FUN_00021500();
  iVar1 = extraout_ECX;
  for (iVar2 = 0; iVar2 <= *(short *)(param_1 + 4); iVar2 = iVar2 + 1) {
    if (*(int **)(iVar1 + 1) != (int *)0x0) {
      (**(code **)(**(int **)(iVar1 + 1) + 0x1c))(param_1,iVar3);
    }
    FUN_000215a0();
    iVar1 = extraout_ECX_00;
    if (*(int **)(extraout_ECX_00 + 1) != (int *)0x0) {
      (**(code **)(**(int **)(extraout_ECX_00 + 1) + 0x1c))();
      iVar1 = extraout_ECX_01;
    }
  }
  return;
}

