
void __regparm2 FUN_0002e2c0(int param_1,int param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int unaff_EBX;
  
  if ((unaff_EBX == 0) && (*(int *)(param_1 + 0x112) != 0)) {
    unaff_EBX = (**(code **)(*(int *)(*(int *)(param_1 + 0x112) + 8) + 0xdc))();
  }
  if (unaff_EBX != 0) {
    *(undefined4 *)(unaff_EBX + 0xb1) = *(undefined4 *)(param_1 + 0xb1);
    uVar1 = (**(code **)(*(int *)(param_1 + 8) + 0x70))();
    *(undefined4 *)(unaff_EBX + 0x6d) = uVar1;
    if (param_2 == 0x1d) {
      FUN_000749a0();
      FUN_00049580();
    }
    else if (param_2 == 0x1f) {
      FUN_000749a0();
      FUN_00049580();
    }
    else if (param_2 == 0x21) {
      FUN_000749a0();
      FUN_00049580();
    }
    else if (param_2 == 0x1e) {
      FUN_000749a0();
      FUN_00049580();
    }
    else if (param_2 == 0x20) {
      FUN_000749a0();
      FUN_00049580();
    }
    else {
      ll_printf(&DAT_000a92d4,s_Blad_przy_rzucie_patrz_zrodla_00090c09);
      ll_rethrow_report();
    }
    FUN_00050c70(param_3);
  }
  return;
}

