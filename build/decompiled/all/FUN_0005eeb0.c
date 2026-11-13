
int * FUN_0005eeb0(int param_1)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  int *extraout_ECX;
  
  piVar3 = (int *)FUN_0005ec20();
  piVar3[9] = (int)&PTR_FUN_00098274;
  piVar3[5] = 0;
  piVar3[6] = 0;
  piVar3[7] = 0;
  piVar3[8] = 0;
  piVar3[10] = param_1;
  piVar3[0xb] = 1;
  piVar3[4] = (int)&PTR_LAB_000a2e2c;
  (*(code *)**(undefined4 **)(*(int *)(*(int *)(piVar3[3] + 8) + 4) + 0x33))();
  iVar1 = *piVar3;
  iVar2 = piVar3[1];
  *extraout_ECX = *extraout_ECX + iVar1;
  extraout_ECX[2] = extraout_ECX[2] + iVar1;
  extraout_ECX[1] = extraout_ECX[1] + iVar2;
  extraout_ECX[3] = extraout_ECX[3] + iVar2;
  return piVar3;
}

