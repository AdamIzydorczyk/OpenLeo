
int __regparm3 FUN_0001e9b0(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined4 *extraout_ECX;
  int unaff_EBX;
  undefined4 uVar2;
  
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if (iVar1 != 0) {
    iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x6c))(0,0x1a4,3000,0x208,&PTR_FUN_00098274);
    iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
    if (iVar1 == 0) {
      uVar2 = *extraout_ECX;
      iVar1 = extraout_ECX[1];
      (*(code *)**(undefined4 **)(**(int **)(extraout_ECX[0xf] + 8) + 0x33))(uVar2,iVar1);
      iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x6c))(uVar2,iVar1 + -6);
      iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
      if (iVar1 != 0) {
        FUN_00052b10();
        FUN_00052b10();
      }
    }
  }
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0();
  return param_2;
}

