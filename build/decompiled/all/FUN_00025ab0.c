
void __regparm3 FUN_00025ab0(int *param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  int *extraout_ECX;
  int *extraout_ECX_00;
  int *piVar3;
  int *extraout_ECX_01;
  undefined4 *puVar4;
  undefined4 *puVar5;
  
  iVar2 = param_1[3];
  param_1[3] = iVar2 + param_2;
  if (iVar2 + param_2 == 0) {
    if (*param_1 != 0) {
      (*(code *)**(undefined4 **)(*param_1 + 8))();
      param_1 = extraout_ECX_01;
    }
    *param_1 = 0;
  }
  else {
    thunk_FUN_0005c320();
    puVar1 = (undefined4 *)FUN_00076bb7();
    iVar2 = 0;
    puVar4 = puVar1;
    if (0 < extraout_ECX[2]) {
      do {
        puVar5 = (undefined4 *)(iVar2 * 0xc + *extraout_ECX);
        *puVar4 = *puVar5;
        puVar4[1] = puVar5[1];
        iVar2 = iVar2 + 1;
        puVar4 = puVar4 + 3;
      } while (iVar2 < extraout_ECX[2]);
    }
    piVar3 = extraout_ECX;
    if (*extraout_ECX != 0) {
      (*(code *)**(undefined4 **)(*extraout_ECX + 8))();
      piVar3 = extraout_ECX_00;
    }
    *piVar3 = (int)puVar1;
  }
  return;
}

