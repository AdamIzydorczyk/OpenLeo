
undefined4 * __regparm3 FUN_0005d170(undefined4 *param_1,undefined4 *param_2)

{
  code *pcVar1;
  int iVar2;
  undefined **ppuVar3;
  undefined *puStack_28;
  undefined4 local_24;
  undefined **local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 *local_14;
  
  local_1c = 0;
  local_18 = 0;
  puStack_28 = (undefined *)0x5d190;
  local_14 = param_2;
  FUN_0005b860();
  puStack_28 = (undefined *)0x5d19f;
  FUN_0005b9e0();
  puStack_28 = (undefined *)0x5d1a4;
  ll_init_pair();
  puStack_28 = (undefined *)0x5d1b0;
  FUN_0001cc10();
  puStack_28 = (undefined *)0x5d1bc;
  FUN_00056380();
  puStack_28 = (undefined *)0x5d1c6;
  FUN_00055cf0();
  puStack_28 = (undefined *)0x5d1d6;
  FUN_0001cc10();
  puStack_28 = (undefined *)0x5d1e6;
  FUN_00056380();
  puStack_28 = (undefined *)0x5d1f0;
  FUN_00076bf0();
  puStack_28 = (undefined *)0x5d1fa;
  iVar2 = FUN_0005c320();
  ppuVar3 = (undefined **)&local_24;
  if (iVar2 != 0) {
    puStack_28 = &DAT_0009262e;
    local_24 = 0;
    local_20 = &PTR_FUN_00096200;
    FUN_00057990();
    local_20 = &PTR_FUN_000961fc;
    FUN_0004a4c0();
    ppuVar3 = &puStack_28;
  }
  *(undefined4 *)((int)ppuVar3 + -4) = 0x5d245;
  FUN_00057a00();
  pcVar1 = *(code **)(*(int *)*param_1 + 0x28);
  *(undefined4 *)((int)ppuVar3 + -4) = 0x5d24c;
  (*pcVar1)();
  *(undefined4 *)((int)ppuVar3 + -4) = 0x5d256;
  local_1c = FUN_0004abd0();
  local_18 = *(undefined4 *)(DAT_000cef40 + 4);
  *(undefined4 *)((int)ppuVar3 + -4) = 0x5d26e;
  FUN_0005b8f0();
  *(undefined4 *)((int)ppuVar3 + -4) = 0x5d278;
  FUN_00055d20();
  *(undefined4 *)((int)ppuVar3 + -4) = 0x5d288;
  FUN_0001cc10();
  *(undefined4 *)((int)ppuVar3 + -4) = 0x5d298;
  FUN_00056380();
  if (DAT_000cef40 != 0) {
    pcVar1 = *(code **)(*(int *)(DAT_000cef40 + 0x3e6) + 4);
    *(undefined4 *)((int)ppuVar3 + -4) = 0x5d2af;
    (*pcVar1)();
  }
  DAT_000cef40 = 0;
  *(undefined4 *)((int)ppuVar3 + -4) = 0x5d2c1;
  FUN_0001c490();
  while( true ) {
    *(undefined4 *)((int)ppuVar3 + -4) = 0x5d2c6;
    iVar2 = FUN_000766a7();
    if (iVar2 == 0) break;
    *(undefined4 *)((int)ppuVar3 + -4) = 0x5d2cf;
    FUN_000766be();
  }
  *local_14 = local_1c;
  local_14[1] = local_18;
  return local_14;
}

