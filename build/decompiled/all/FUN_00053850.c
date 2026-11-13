
/* WARNING: Removing unreachable block (ram,0x00053922) */
/* WARNING: Removing unreachable block (ram,0x00053932) */
/* WARNING: Removing unreachable block (ram,0x00053938) */
/* WARNING: Removing unreachable block (ram,0x00053944) */
/* WARNING: Removing unreachable block (ram,0x0005394e) */

void __regparm1 FUN_00053850(undefined4 *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 extraout_ECX;
  int iVar2;
  
  FUN_00057990();
  *param_1 = extraout_ECX;
  param_1[1] = param_2;
  *(undefined4 *)((int)param_1 + 0x89) = extraout_ECX;
  *(undefined4 *)((int)param_1 + 0x8d) = param_2;
  *(undefined4 *)((int)param_1 + 0x6d) = 1;
  *(undefined4 *)((int)param_1 + 0x71) = 1;
  *(undefined4 *)((int)param_1 + 0x99) = 1;
  *(undefined4 *)((int)param_1 + 0xa1) = 0;
  param_1[0x17] = 1;
  FUN_000749a0();
  *(undefined4 *)((int)param_1 + 0x95) = 1;
  *(undefined4 *)((int)param_1 + 0x9d) = 0;
  FUN_00049820();
  FUN_00057d30();
  param_1[0x16] = param_1[0x11];
  uVar1 = thunk_FUN_0005c320();
  param_1[0x14] = uVar1;
  FUN_000749a0();
  iVar2 = 0;
  if (0 < (int)param_1[0x16]) {
    do {
      FUN_00057d30();
      iVar2 = iVar2 + 1;
      FUN_00053480();
    } while (iVar2 < (int)param_1[0x16]);
  }
  (**(code **)(param_1[2] + 0x2c))();
  FUN_00057990();
  (**(code **)(*(int *)(*(int *)(param_1[0x17] * 4 + param_1[0x14]) + 0x44) + 0x1c))();
  FUN_00057a00();
  FUN_00049a10();
  FUN_00057a00();
  return;
}

