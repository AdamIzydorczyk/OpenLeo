
/* drive-presence AND chain; pass counter [0xcf1c0] */

void __regparm3 ll_cdcheck_chain(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = ll_cd_int2f(param_2);
  if (iVar1 != 0) {
    iVar1 = ll_cd_ioctl_4408();
    if (iVar1 == 0) {
      iVar1 = ll_cd_ioctl_4409();
      if (iVar1 == 0) {
        iVar1 = ll_stub_true();
        if (iVar1 == 0) {
          iVar1 = 1;
          goto LAB_0005bd8a;
        }
      }
    }
  }
  iVar1 = 0;
LAB_0005bd8a:
  DAT_000cf1c0 = DAT_000cf1c0 + iVar1;
  return;
}

