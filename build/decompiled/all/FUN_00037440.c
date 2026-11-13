
int __regparm3 FUN_00037440(undefined4 param_1,int param_2)

{
  int iVar1;
  int *extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int unaff_EBX;
  
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if (iVar1 != 0) {
    iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x6c))
                      (*extraout_ECX + -10,extraout_ECX[1] + -10,*extraout_ECX + 10,
                       extraout_ECX[1] + 10,&PTR_FUN_00098274);
    iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
    if (iVar1 != 0) {
      iVar1 = extraout_ECX_00;
      if (*(int *)(extraout_ECX_00 + 0x44) != -1) {
        FUN_00052cc0();
        iVar1 = extraout_ECX_01;
      }
      (**(code **)(*(int *)(iVar1 + 8) + 0x84))();
      FUN_00052b10();
    }
  }
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0();
  return param_2;
}

