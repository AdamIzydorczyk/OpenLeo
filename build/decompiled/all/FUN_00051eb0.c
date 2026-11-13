
void __regparm3 FUN_00051eb0(undefined4 *param_1,undefined4 param_2)

{
  code *pcVar1;
  bool bVar2;
  int iVar3;
  undefined4 uVar4;
  undefined ***pppuVar5;
  undefined **appuStack_28 [6];
  
  pppuVar5 = appuStack_28 + 2;
  *(undefined2 *)(param_1 + 3) = 0;
  *(undefined2 *)((int)param_1 + 10) = *(undefined2 *)(param_1 + 3);
  *(undefined2 *)(param_1 + 2) = *(undefined2 *)(param_1 + 3);
  *param_1 = param_2;
  appuStack_28[2] = (undefined **)0x0;
  appuStack_28[3] = &PTR_FUN_000961fc;
  appuStack_28[5] = (undefined **)param_1;
  while( true ) {
    pppuVar5[-1] = (undefined **)0x51efb;
    FUN_00057d30();
    if ((*pppuVar5 == (undefined **)0x0) || (*(char *)*pppuVar5 == '\0')) {
      bVar2 = true;
    }
    else {
      bVar2 = false;
    }
    if (bVar2) break;
    pppuVar5[-1] = (undefined **)0x51f23;
    iVar3 = FUN_00057db0();
    if (iVar3 == 0) {
      pcVar1 = (code *)**(undefined4 **)((int)pppuVar5[3] + 0x44);
      pppuVar5[-1] = (undefined **)0x51fa5;
      (*pcVar1)();
    }
    else {
      pppuVar5[-1] = (undefined **)0x51f31;
      iVar3 = FUN_0005c320();
      uVar4 = 0;
      if (iVar3 != 0) {
        pppuVar5[-1] = (undefined **)*(undefined4 *)((int)pppuVar5[3] + 0xe);
        pppuVar5[-2] = (undefined **)0x51f48;
        FUN_00057fd0();
        pppuVar5[-2] = (undefined **)0x51f59;
        uVar4 = FUN_000515e0();
        pppuVar5 = pppuVar5 + -1;
      }
      *(undefined4 *)((int)pppuVar5 + 8) = uVar4;
      if (*(int *)((int)param_1 + 0x36) == *(int *)((int)param_1 + 0x3a)) {
        *(undefined4 *)((int)pppuVar5 + -4) = 0x51f77;
        FUN_00021e80();
      }
      iVar3 = *(int *)((int)param_1 + 0x36);
      *(int *)((int)param_1 + 0x36) = iVar3 + 1;
      *(undefined4 *)(*(int *)((int)param_1 + 0x2e) + iVar3 * 4) =
           *(undefined4 *)((int)pppuVar5 + 8);
    }
  }
  if (*(int *)((int)pppuVar5[3] + 0x36) < *(int *)((int)pppuVar5[3] + 0x3a)) {
    pppuVar5[-1] = (undefined **)0x51fc5;
    FUN_00021e80();
  }
  if (*(int *)((int)pppuVar5[3] + 0x22) < *(int *)((int)pppuVar5[3] + 0x26)) {
    pppuVar5[-1] = (undefined **)0x51fe0;
    FUN_00022010();
  }
  pppuVar5[-1] = (undefined **)0x51fec;
  FUN_00057a00();
  return;
}

