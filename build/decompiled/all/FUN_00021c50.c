
void __regparm3 FUN_00021c50(int *param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = param_1[3];
  param_1[3] = iVar1 + param_2;
  if (iVar1 + param_2 == 0) {
    if (*param_1 != 0) {
      (*(code *)**(undefined4 **)(*param_1 + 4))();
    }
    *param_1 = 0;
  }
  else {
    thunk_FUN_0005c320();
    iVar1 = FUN_00076bb7();
    iVar2 = 0;
    if (0 < param_1[2]) {
      do {
        iVar2 = iVar2 + 1;
        FUN_00057990();
      } while (iVar2 < param_1[2]);
    }
    if (*param_1 != 0) {
      (*(code *)**(undefined4 **)(*param_1 + 4))();
    }
    *param_1 = iVar1;
  }
  return;
}

