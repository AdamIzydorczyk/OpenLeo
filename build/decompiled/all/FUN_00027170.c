
void __regparm3 FUN_00027170(int param_1,int *param_2)

{
  int *piVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  undefined4 *unaff_EBX;
  
  if ((*param_2 == 6) && (0 < *(int *)(param_1 + 0x10e) * *(int *)(param_1 + 0x6d))) {
    FUN_00053670();
    param_1 = extraout_ECX;
  }
  if ((*(int *)(param_1 + 0x5c) == 0x12) &&
     (*(int *)(*(int *)(*(int *)(param_1 + 0x50) + 0x48) + 4) == 2)) {
    piVar1 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x68))();
    (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x10))
              (0,1,0x143 - *piVar1,0x18b - piVar1[1],*piVar1,piVar1[1],&PTR_LAB_00098278);
    param_1 = extraout_ECX_01;
  }
  if ((*(byte *)(param_1 + 0x60) & 0x40) != 0) {
    *param_2 = 2;
    *unaff_EBX = 0;
  }
  if (*(int *)(param_1 + 0x11a) == 8) {
    *(undefined4 *)(param_1 + 0x11a) = 0;
    *param_2 = 0x17;
    *unaff_EBX = 0;
  }
  else {
    FUN_00050970();
  }
  return;
}

