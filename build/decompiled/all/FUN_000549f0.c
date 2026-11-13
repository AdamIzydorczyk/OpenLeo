
void __regparm3 FUN_000549f0(int param_1,undefined4 *param_2)

{
  uint *puVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  undefined4 *extraout_ECX;
  undefined4 *extraout_ECX_00;
  undefined4 *extraout_ECX_01;
  undefined4 *extraout_ECX_02;
  undefined4 *extraout_ECX_03;
  undefined4 *extraout_ECX_04;
  undefined8 uVar5;
  
  *(undefined4 *)(param_1 + 0xba) = *param_2;
  iVar4 = *(int *)(param_1 + 0xd2);
  *(undefined4 *)(param_1 + 0xbe) = param_2[1];
  if (iVar4 == 0) goto LAB_00054b90;
  iVar2 = *(int *)(iVar4 + 4);
  if (iVar2 == 1) {
    uVar5 = (**(code **)(*(int *)(param_1 + 8) + 0x68))();
    if (*(int *)uVar5 + 5 < *(int *)(*(int *)(param_1 + 0xd2) + 8) + (int)((ulonglong)uVar5 >> 0x20)
       ) {
      *(undefined4 *)(param_1 + 0xba) = 10;
      param_2 = extraout_ECX;
      goto LAB_00054b90;
    }
    uVar5 = (**(code **)(*(int *)(param_1 + 8) + 0x68))();
    if (*(int *)(*(int *)(param_1 + 0xd2) + 8) - (int)((ulonglong)uVar5 >> 0x20) <
        *(int *)uVar5 + -5) {
      *(undefined4 *)(param_1 + 0xba) = 0xfffffff6;
      param_2 = extraout_ECX_00;
      goto LAB_00054b90;
    }
    *(undefined4 *)(param_1 + 0xba) = 0;
  }
  else if (iVar2 == 4) {
    piVar3 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x68))();
    if (*piVar3 + 8 < *(int *)(*(int *)(param_1 + 0xd2) + 8)) {
      *(undefined4 *)(param_1 + 0xba) = 10;
    }
    else {
      piVar3 = (int *)(**(code **)(*(int *)(param_1 + 8) + 0x68))();
      if (*(int *)(*(int *)(param_1 + 0xd2) + 8) < *piVar3 + -8) {
        *(undefined4 *)(param_1 + 0xba) = 0xfffffff6;
      }
      else {
        *(undefined4 *)(param_1 + 0xba) = 0;
      }
    }
    iVar4 = (**(code **)(*(int *)(param_1 + 8) + 0x68))();
    if (*(int *)(iVar4 + 4) + 8 < *(int *)(*(int *)(param_1 + 0xd2) + 0x1d)) {
      *(undefined4 *)(param_1 + 0xbe) = 10;
      param_2 = extraout_ECX_01;
    }
    else {
      iVar4 = (**(code **)(*(int *)(param_1 + 8) + 0x68))();
      param_2 = extraout_ECX_02;
      if (*(int *)(*(int *)(param_1 + 0xd2) + 0x1d) < *(int *)(iVar4 + 4) + -8) {
        *(undefined4 *)(param_1 + 0xbe) = 0xfffffff6;
      }
      else {
        *(undefined4 *)(param_1 + 0xbe) = 0;
      }
    }
    if ((*(int *)(param_1 + 0xba) != 0) || (*(int *)(param_1 + 0xbe) != 0)) goto LAB_00054b90;
  }
  else {
    if ((iVar2 != 2) ||
       ((*(int *)(iVar4 + 8) != 0 && (*(int *)(iVar4 + 8) != *(int *)(param_1 + 0x5c)))))
    goto LAB_00054b90;
    FUN_00049580();
  }
  FUN_000551f0();
  param_2 = extraout_ECX_03;
LAB_00054b90:
  if (*(int *)(param_1 + 0xbe) < 1) {
    if (*(int *)(param_1 + 0xbe) < 0) {
      *(undefined4 *)(param_1 + 0x71) = 1;
    }
  }
  else {
    *(undefined4 *)(param_1 + 0x71) = 0xffffffff;
  }
  puVar1 = (uint *)(param_1 + 0xb6);
  *(byte *)puVar1 = *(byte *)puVar1 & 0xef;
  *puVar1 = *puVar1 | (uint)(*(int *)(param_1 + 0xba) * *(int *)(param_1 + 0x6d) < 0) << 4;
  if ((*(byte *)(param_1 + 0xb6) & 0x10) != 0) {
    FUN_00053670();
    param_2 = extraout_ECX_04;
  }
  *(undefined4 *)(param_1 + 0xa5) = param_2[2];
  return;
}

