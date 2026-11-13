
void __regparm3 FUN_000592e0(int param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 **ppuVar3;
  undefined4 *puStack_2c;
  undefined1 local_28 [4];
  int local_24;
  undefined4 local_18;
  undefined **local_14;
  
  puStack_2c = (undefined4 *)0x592f2;
  iVar1 = FUN_000592d0();
  *(int *)(param_1 + 8) = iVar1;
  if (iVar1 == 0) {
    puStack_2c = param_2;
    ll_printf(&DAT_000a92d4,s_Could_not_open_resource_file___s_00092343);
    puStack_2c = (undefined4 *)0x59316;
    ll_rethrow_report();
  }
  ppuVar3 = (undefined4 **)local_28;
  puStack_2c = (undefined4 *)0x5932a;
  FUN_000773e4();
  *(undefined4 *)(param_1 + 0x2c) = 0;
  puStack_2c = (undefined4 *)0x59340;
  FUN_00059ba0();
  puStack_2c = (undefined4 *)0x59352;
  iVar1 = FUN_00077010();
  if (iVar1 != 0) {
    puStack_2c = param_2;
    ll_printf(&DAT_000a92d4,s_Invalid_resource_file___s_00092369);
    puStack_2c = (undefined4 *)0x59373;
    ll_rethrow_report();
  }
  if (1 < local_24) {
    puStack_2c = param_2;
    ll_printf(&DAT_000a92d4,s_Unknown_version_of_resource_file_00092383);
    puStack_2c = (undefined4 *)0x59396;
    ll_rethrow_report();
  }
  puStack_2c = (undefined4 *)0x593a6;
  FUN_000765ab();
  puStack_2c = (undefined4 *)0x593ae;
  uVar2 = FUN_00076527();
  *(undefined4 *)(param_1 + 0x28) = uVar2;
  puStack_2c = (undefined4 *)0x593bb;
  iVar1 = FUN_0005c320();
  uVar2 = 0;
  if (iVar1 != 0) {
    local_14 = &PTR_FUN_00096200;
    local_18 = 0;
    puStack_2c = (undefined4 *)0x593db;
    FUN_00057990();
    puStack_2c = &local_18;
    local_14 = &PTR_FUN_000961fc;
    uVar2 = FUN_00058b40();
    ppuVar3 = &puStack_2c;
  }
  *(undefined4 *)(param_1 + 4) = uVar2;
  *(undefined4 *)((int)ppuVar3 + -4) = 0x59409;
  FUN_00057a00();
  return;
}

