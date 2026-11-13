
/* entry via 0x78afa; game bootstrap: ctx save, CD check, free-mem msg, handler install, koniec.is
    */

void ll_main(void)

{
  bool bVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 extraout_EDX;
  undefined4 *extraout_EDX_00;
  undefined2 in_DS;
  int local_44;
  
  DAT_000ceed8 = FUN_00076dda();
  if (DAT_000ceed8 == 0) {
    FUN_0005c740();
    DAT_000cef14 = &DAT_000cf3d8;
    FUN_0005bf10();
    FUN_00057990();
    FUN_00057a00();
    if ((DAT_000cf3cc == (char *)0x0) || (*DAT_000cf3cc == '\0')) {
      bVar1 = true;
    }
    else {
      bVar1 = false;
    }
    if ((bVar1) || (iVar2 = ll_cdcheck_chain(), iVar2 == 0)) {
      uVar3 = ll_msg_get();
      ll_printf(&DAT_000a92d4,uVar3);
      ll_rethrow_report();
    }
    iVar2 = FUN_0005c430();
    if (iVar2 + -0x546190 < 0) {
      uVar3 = ll_msg_get((-(iVar2 + -0x546190) / 0x2800) * 10);
      ll_printf(&DAT_000a92d4,uVar3);
      ll_rethrow_report();
    }
    FUN_00077ad2();
    DAT_000cf458 = 1;
    FUN_00057a70();
    iVar2 = FUN_0007607b();
    FUN_00057a00();
    if ((iVar2 == 0) || (DAT_000cf45c != 0)) {
      uVar3 = ll_msg_get();
      ll_printf(&DAT_000a92d4,uVar3);
      ll_rethrow_report();
    }
    FUN_0007630a();
    DAT_000cf458 = 0;
    DAT_000cf450 = extraout_EDX;
    DAT_000cf454 = in_DS;
    FUN_0005bb20();
    FUN_0005bad0();
    FUN_0005bb20();
    FUN_0005bb20();
    FUN_00059cb0();
    FUN_0005a5d0();
    FUN_00059cb0();
    FUN_0005a5d0();
    FUN_0005ce20();
    FUN_00057a70();
    FUN_00059cb0();
    FUN_00057a70();
    FUN_0005a5d0();
    FUN_00057a00();
    FUN_00057a00();
    FUN_0005ce30();
    FUN_00057a70();
    FUN_00059cb0();
    FUN_00057a70();
    FUN_0005a5d0();
    FUN_00057a00();
    FUN_00057a00();
    FUN_0005ce40();
    FUN_00057a70();
    FUN_00059cb0();
    FUN_00057a70();
    FUN_0005a5d0();
    FUN_00057a00();
    FUN_00057a00();
    FUN_0005ce50();
    FUN_00057a70();
    FUN_00059cb0();
    FUN_00057a70();
    FUN_0005a5d0();
    FUN_00057a00();
    FUN_00057a00();
    FUN_0005c6c0();
    FUN_0005c560();
    FUN_0005e280();
    if (DAT_000a9894 == '\0') {
      if (DAT_000cf3d4 != 0) {
        FUN_0005e370();
      }
      FUN_0005e410();
    }
    else {
      FUN_0005eae0();
    }
    FUN_0005e310();
    FUN_0001c740();
    FUN_0001c740();
    if (local_44 != 0) {
      FUN_0005baf0();
    }
    FUN_0001c740();
  }
  if ((DAT_000ceed8 != 0) && (DAT_000ceed8 = DAT_000ceed8 + -1, DAT_000ceed8 == 0)) {
    FUN_0004a270();
    (**(code **)*extraout_EDX_00)();
    FUN_00077baf();
    iVar2 = FUN_0005bda0();
    if (iVar2 != 0) {
      FUN_00077baf();
      FUN_000766be();
    }
  }
  if ((DAT_000ceed8 != 0) && (DAT_000ceed8 = DAT_000ceed8 + -1, DAT_000ceed8 == 0)) {
    FUN_00077baf();
  }
  return;
}

