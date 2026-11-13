
void FUN_00055530(void)

{
  int iVar1;
  undefined4 *extraout_ECX;
  int *unaff_EBX;
  
  iVar1 = FUN_0004dcc0();
  *unaff_EBX = iVar1;
  (**(code **)(*(int *)(iVar1 + 8) + 0x10))(0,1,*extraout_ECX,extraout_ECX[1]);
  (**(code **)(*(int *)(*unaff_EBX + 8) + 0x10))();
  return;
}

