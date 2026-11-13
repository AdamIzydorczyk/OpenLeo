
void __regparm3 FUN_0002b560(int *param_1,undefined4 param_2)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar2;
  undefined4 *puVar3;
  byte bVar4;
  undefined4 local_20;
  undefined4 local_1c [2];
  
  bVar4 = 0;
  local_1c[1] = param_2;
  FUN_00049920(param_2,&DAT_00090abc,param_1);
  if (0 < *param_1) {
    do {
      FUN_00049920(local_1c[1],s__d____d_00090ac1,&local_20,local_1c);
      iVar2 = extraout_ECX;
      if (param_1[5] == param_1[6]) {
        FUN_0002d9d0();
        iVar2 = extraout_ECX_00;
      }
      iVar1 = param_1[5];
      param_1[5] = iVar1 + 1;
      puVar3 = (undefined4 *)(iVar1 * 8 + param_1[3]);
      *puVar3 = local_20;
      puVar3[(uint)bVar4 * -2 + 1] = local_1c[(uint)bVar4 * -2];
    } while (iVar2 + 1 < *param_1);
  }
  FUN_0002b5f0();
  return;
}

