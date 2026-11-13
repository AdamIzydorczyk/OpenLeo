
void __regparm3 FUN_0002b9c0(int param_1)

{
  int iVar1;
  int extraout_ECX;
  
  FUN_0002b860(0xfffea070,0xfffea070,90000,90000,&PTR_FUN_00098274);
  if (*(int *)(extraout_ECX + 0x12e) == 0) {
    *(undefined4 *)(param_1 + 0x2c) = 0;
  }
  iVar1 = *(int *)(extraout_ECX + 0x12a);
  *(undefined4 *)(extraout_ECX + 0x12a) = 0xffffffff;
  if (-1 < iVar1) {
    FUN_0002b840();
  }
  return;
}

