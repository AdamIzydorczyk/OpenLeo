
void __regparm3 FUN_0005c370(int param_1)

{
  int iVar1;
  int *piVar2;
  int *piVar3;
  int extraout_EDX;
  
  if (param_1 != 0) {
    while (iVar1 = DAT_000cf2cc * 4, param_1 == *(int *)(&DAT_000cf2d0 + iVar1)) {
      DAT_000cf2cc = DAT_000cf2cc + -1;
      piVar3 = (int *)0x0;
      for (piVar2 = DAT_000cf2c8; piVar2 != (int *)0x0; piVar2 = (int *)piVar2[2]) {
        if (*piVar2 == *(int *)(&DAT_000cf2d0 + iVar1)) goto LAB_0005c3da;
        piVar3 = piVar2;
      }
      ll_printf(&DAT_000a92d4,s_Bledne_zwolnienie_zalockowanego_b_00092514);
      ll_rethrow_report();
      piVar3 = (int *)0x0;
LAB_0005c3da:
      if (piVar3 == (int *)0x0) {
        piVar3 = (int *)&DAT_000cf2c8;
      }
      else {
        piVar3 = piVar3 + 2;
      }
      iVar1 = *piVar3;
      *piVar3 = *(int *)(iVar1 + 8);
      if (iVar1 == 0) {
        return;
      }
      FUN_00075302();
      if (*(int *)(extraout_EDX + 4) != 0) {
        FUN_0005c500();
        FUN_0005c370();
      }
      param_1 = extraout_EDX;
      if (extraout_EDX == 0) {
        return;
      }
    }
    FUN_00075302();
  }
  return;
}

