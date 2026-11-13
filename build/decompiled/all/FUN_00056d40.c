
/* WARNING: Removing unreachable block (ram,0x00056e6f) */
/* WARNING: Removing unreachable block (ram,0x00056d9e) */
/* WARNING: Removing unreachable block (ram,0x00056d73) */
/* WARNING: Removing unreachable block (ram,0x00056d85) */
/* WARNING: Removing unreachable block (ram,0x00056dd0) */
/* WARNING: Removing unreachable block (ram,0x00056ec8) */

void __regparm3 FUN_00056d40(short *param_1)

{
  undefined4 uVar1;
  int iVar2;
  int extraout_ECX;
  int iVar3;
  
  FUN_00049aa0();
  uVar1 = thunk_FUN_0005c320();
  *(undefined4 *)(param_1 + 9) = uVar1;
  if (-1 < (param_1[1] + -1) / (int)param_1[6]) {
    uVar1 = thunk_FUN_0005c320();
    *(undefined4 *)(param_1 + 7) = uVar1;
    iVar2 = 0;
    for (iVar3 = 0; iVar3 < *param_1; iVar3 = iVar3 + 1) {
      *(undefined4 *)(iVar2 + *(int *)(param_1 + 7)) = 0;
      uVar1 = thunk_FUN_0005c320();
      *(undefined4 *)(extraout_ECX + *(int *)(param_1 + 7)) = uVar1;
      **(undefined4 **)(extraout_ECX + *(int *)(param_1 + 7)) = 0;
      iVar2 = extraout_ECX + 4;
    }
  }
  uVar1 = thunk_FUN_0005c320();
  *(undefined4 *)(param_1 + 2) = uVar1;
  FUN_00049af0();
  FUN_00057a00();
  return;
}

