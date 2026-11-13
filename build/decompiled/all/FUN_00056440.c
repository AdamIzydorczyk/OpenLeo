
undefined4 __regparm3 FUN_00056440(int param_1,undefined4 param_2,int *param_3)

{
  undefined4 uVar1;
  char unaff_BL;
  
  if (unaff_BL == '\0') {
    uVar1 = FUN_000564a0();
    return uVar1;
  }
  if (unaff_BL != '\x01') {
    ll_printf(&DAT_000a92d4,s_Unknown_channel_request_000922e7);
    ll_rethrow_report();
    return 0;
  }
  if (*param_3 != -1) {
    *(undefined4 *)(*(int *)(param_1 + 0x40) + *param_3 * 4) = DAT_000cf0e4;
    *param_3 = -1;
  }
  return 1;
}

