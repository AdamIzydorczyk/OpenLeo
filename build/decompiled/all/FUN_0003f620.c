
void __regparm3 FUN_0003f620(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  undefined4 extraout_ECX;
  undefined4 extraout_ECX_00;
  undefined4 extraout_ECX_01;
  undefined1 local_10 [4];
  undefined1 local_c [4];
  undefined4 uStack_8;
  
  uStack_8 = param_3;
  FUN_00044f10();
  FUN_00049920(extraout_ECX,s_sample____d_000914f9,param_1 + 0x4c);
  FUN_00049920(extraout_ECX_00,s_prior____d_00091506,local_10);
  FUN_00049920(extraout_ECX_01,s_loop____x_00091512,local_c);
  iVar2 = *(int *)(param_1 + 0x48);
  if ((iVar2 != DAT_000cf0e4) && (iVar2 != 0)) {
    (**(code **)(*(int *)(iVar2 + 8) + 0x14))();
  }
  iVar1 = FUN_0005c320();
  iVar2 = 0;
  if (iVar1 != 0) {
    iVar2 = FUN_00056620();
  }
  *(int *)(param_1 + 0x48) = iVar2;
  (**(code **)(*(int *)(iVar2 + 8) + 0x2c))();
  return;
}

