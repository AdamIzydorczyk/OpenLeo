
/* INT21 AX=4409 IoControl */

undefined8 __regparm3 ll_cd_ioctl_4409(undefined1 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  byte local_17;
  byte local_c;
  
  FUN_00075c3d(0x4409,param_1);
  if (((local_c & 1) == 0) && ((local_17 & 0x80) != 0)) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return CONCAT44(param_2,uVar1);
}

