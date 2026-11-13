
void __regparm3 FUN_00054250(int param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar6;
  
  iVar5 = (**(code **)(*(int *)(param_1 + 8) + 0xbc))();
  if (((iVar5 != 0) && (param_2[2] < 0x14)) && ((*(byte *)(extraout_ECX + 0xb6) & 2) == 0)) {
    param_2[2] = 0x14;
  }
  iVar5 = param_2[2];
  iVar1 = *(int *)(extraout_ECX + 0xc2);
  iVar2 = *(int *)(extraout_ECX + 0xc6);
  iVar3 = param_2[3];
  *(int *)(extraout_ECX + 0xc2) =
       *(int *)(extraout_ECX + 0xc2) +
       (*param_2 * *(int *)(extraout_ECX + 0xba)) / 0x14 +
       *(int *)(extraout_ECX + 0x6d) * param_2[4];
  iVar6 = *(int *)(extraout_ECX + 0xc6) +
          (param_2[1] * *(int *)(extraout_ECX + 0xbe)) / 0x14 + param_2[5];
  iVar4 = *(int *)(extraout_ECX + 0xc2);
  *(int *)(extraout_ECX + 0xc6) = iVar6;
  *(int *)(extraout_ECX + 0xc2) = iVar4 - (iVar5 * iVar1) / 100;
  *(int *)(extraout_ECX + 0xc6) = *(int *)(extraout_ECX + 0xc6) - (iVar3 * iVar2) / 100;
  if (iVar4 * *(int *)(extraout_ECX + 0xc2) < 0) {
    *(undefined4 *)(extraout_ECX + 0xc2) = 0;
  }
  if (iVar6 * *(int *)(extraout_ECX + 0xc6) < 0) {
    *(undefined4 *)(extraout_ECX + 0xc6) = 0;
  }
  FUN_00054170();
  *(int *)(extraout_ECX_00 + 0xca) = *(int *)(extraout_ECX_00 + 0xc2) / 100;
  *(int *)(extraout_ECX_00 + 0xce) = *(int *)(extraout_ECX_00 + 0xc6) / 100;
  return;
}

