
void __regparm3 FUN_0002b600(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int *extraout_ECX;
  int *extraout_ECX_00;
  int *piVar3;
  int iVar4;
  int local_20;
  int local_1c;
  int local_18;
  
  FUN_00049920(param_2,&DAT_00090acb,&local_20);
  iVar4 = 0;
  if (0 < local_20) {
    do {
      iVar1 = FUN_0005c320();
      iVar2 = 0;
      piVar3 = extraout_ECX;
      if (iVar1 != 0) {
        iVar2 = FUN_0002db30();
        iVar2 = iVar2 + -0xc;
        FUN_0002b560();
        piVar3 = extraout_ECX_00;
      }
      local_1c = iVar2;
      if (piVar3[2] == piVar3[3]) {
        FUN_0002d840();
      }
      iVar2 = piVar3[2];
      piVar3[2] = iVar2 + 1;
      *(int *)(iVar2 * 4 + *piVar3) = local_1c;
      iVar4 = iVar4 + 1;
    } while (iVar4 < local_20);
  }
  *(undefined4 *)(local_18 + 0xa5) = 0;
  return;
}

