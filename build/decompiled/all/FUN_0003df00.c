
undefined4 __regparm3 FUN_0003df00(int param_1,undefined4 param_2)

{
  int iVar1;
  int unaff_EBX;
  
  if (unaff_EBX != param_1) {
    iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0xd8))();
    if (iVar1 != 0) {
      FUN_00045320();
    }
  }
  FUN_000552e0();
  return param_2;
}

