
void __regparm3 FUN_000249d0(int *param_1,undefined4 param_2)

{
  int *piVar1;
  uint uVar2;
  int iVar3;
  
  FUN_000550a0();
  piVar1 = (int *)(**(code **)(*(int *)(*(int *)(DAT_000cef40 + 0x394) + 8) + 0x68))();
  iVar3 = *piVar1;
  if (((*param_1 < iVar3) && (*(int *)((int)param_1 + 0x6d) < 0)) ||
     ((iVar3 < *param_1 && (0 < *(int *)((int)param_1 + 0x6d))))) {
    FUN_00053670(iVar3,piVar1[1],&PTR_LAB_00098278,param_2);
  }
  uVar2 = *param_1 - iVar3 >> 0x1f;
  if ((int)((*param_1 - iVar3 ^ uVar2) - uVar2) < 0x96) {
    *(byte *)((int)param_1 + 0x66) = *(byte *)((int)param_1 + 0x66) | 0x40;
  }
  return;
}

