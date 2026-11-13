
void __regparm3 FUN_00021cd0(int *param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  
  if (*param_1 != 0) {
    iVar2 = 0;
    if (0 < param_1[2]) {
      do {
        iVar1 = param_1[4];
        uVar3 = 0;
        FUN_00057990(0,&PTR_FUN_00096200);
        (**(code **)(iVar1 + 4))(uVar3,&PTR_FUN_000961fc);
        iVar2 = iVar2 + 1;
        FUN_00057a00();
      } while (iVar2 < param_1[2]);
    }
    if (*param_1 != 0) {
      (*(code *)**(undefined4 **)(*param_1 + 4))();
    }
    *param_1 = 0;
    param_1[2] = 0;
    param_1[3] = 0;
  }
  return;
}

