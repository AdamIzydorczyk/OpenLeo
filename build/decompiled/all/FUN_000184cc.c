
int __regparm3 FUN_000184cc(undefined4 *param_1,undefined1 param_2,undefined4 param_3)

{
  int iVar1;
  
  DAT_000a881e = *param_1;
  DAT_000a8822 = param_1[1];
  DAT_000a8826 = param_1[2];
  DAT_000a8862 = 0xb;
  DAT_000a882a = param_1[3];
  DAT_000a882e = param_1[4];
  DAT_000a8863 = param_2;
  iVar1 = FUN_0001721d(param_3);
  if (iVar1 != 5) {
    FUN_000170a3();
    FUN_000170c2();
    FUN_00017086();
    iVar1 = FUN_00017db6();
    if (iVar1 == 1) {
      FUN_000172e5();
      FUN_00018455();
      FUN_00018082();
      FUN_0001706e();
      FUN_00016d4a();
      iVar1 = 1;
    }
  }
  return iVar1;
}

