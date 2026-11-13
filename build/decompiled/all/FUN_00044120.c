
undefined4 __regparm3 FUN_00044120(int *param_1)

{
  undefined4 uVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar3;
  
  FUN_00057990();
  FUN_00043d40();
  FUN_00057a00();
  iVar3 = 0;
  iVar2 = extraout_ECX;
  while( true ) {
    if (param_1[2] <= iVar2) {
      FUN_00057a00();
      return 0;
    }
    iVar2 = FUN_00076a69();
    if (iVar2 == 0) break;
    iVar3 = iVar3 + 4;
    iVar2 = extraout_ECX_00 + 1;
  }
  uVar1 = *(undefined4 *)(iVar3 + *param_1);
  FUN_00057a00();
  return uVar1;
}

