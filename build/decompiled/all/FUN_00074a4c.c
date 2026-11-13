
void __regparm3 FUN_00074a4c(uint param_1,int param_2)

{
  int *piVar1;
  code *pcVar2;
  undefined4 *extraout_EDX;
  int unaff_EBX;
  uint local_24;
  
  FUN_00077cd2();
  if (unaff_EBX != 0) {
    FUN_00077cf3();
  }
  piVar1 = *(int **)(param_2 + 4);
  FUN_000749b8();
switchD_00074b4d_caseD_2:
  while( true ) {
    if (local_24 == param_1) {
      *(uint *)(param_2 + 8) = local_24;
      return;
    }
    if (local_24 < param_1) {
      FUN_0007539b();
    }
    FUN_00077cf8();
    FUN_000749b8();
    *(uint *)(param_2 + 8) = local_24;
    pcVar2 = (code *)*extraout_EDX;
    if (pcVar2 == (code *)0x0) break;
    (**(code **)(*(int *)(&DAT_000cf518 + *piVar1 * 4) + 4))();
    (*pcVar2)();
  }
  switch(*(undefined2 *)extraout_EDX[1]) {
  default:
    FUN_0007539b();
  case 0:
    (**(code **)(*(int *)(&DAT_000cf518 + *piVar1 * 4) + 4))();
    goto LAB_00074aa4;
  case 2:
  case 3:
  case 4:
    goto switchD_00074b4d_caseD_2;
  case 6:
    break;
  case 7:
    break;
  case 8:
    break;
  case 9:
LAB_00074aa4:
    FUN_00076c00();
    goto switchD_00074b4d_caseD_2;
  case 10:
    goto LAB_00074ae7;
  case 0xb:
LAB_00074ae7:
    (**(code **)((undefined2 *)extraout_EDX[1] + 3))();
    goto switchD_00074b4d_caseD_2;
  }
  FUN_00074a02();
  goto switchD_00074b4d_caseD_2;
}

