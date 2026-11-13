
undefined4 __regparm3 FUN_0005a260(undefined4 param_1,int param_2)

{
  int iVar1;
  int extraout_ECX;
  undefined4 uVar2;
  
  if (param_2 == 0) {
    uVar2 = 0;
  }
  else {
    uVar2 = 0;
    FUN_00057990(0,&PTR_FUN_00096200);
    iVar1 = FUN_00059ff0(uVar2,&PTR_FUN_000961fc);
    uVar2 = (**(code **)(**(int **)(*(int *)(extraout_ECX + 0x14) + iVar1 * 0x10) + 0x10))();
    FUN_00057a00();
  }
  return uVar2;
}

