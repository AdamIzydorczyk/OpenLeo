
undefined4 __regparm3 FUN_0001ee20(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  undefined4 uVar6;
  int extraout_ECX;
  int extraout_ECX_00;
  int unaff_EBX;
  
  iVar4 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if (iVar4 != 0) {
    piVar5 = (int *)(**(code **)(*(int *)(unaff_EBX + 8) + 0x4c))();
    iVar4 = *piVar5;
    iVar1 = piVar5[1];
    iVar2 = piVar5[2];
    iVar3 = piVar5[3];
    FUN_0004a050();
    piVar5 = (int *)(**(code **)(*(int *)(extraout_ECX + 8) + 0x68))();
    if ((((*piVar5 + -100 < iVar4) || (iVar2 < *piVar5 + -100)) || (piVar5[1] < iVar1)) ||
       (iVar3 < piVar5[1])) {
      uVar6 = 0;
    }
    else {
      uVar6 = 1;
    }
    *(undefined4 *)(extraout_ECX_00 + 0x128) = uVar6;
  }
  FUN_000552e0();
  return param_2;
}

