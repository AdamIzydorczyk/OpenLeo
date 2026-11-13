
/* INT21 AX=4408 IoControl start/stop */

undefined8 __regparm3 ll_cd_ioctl_4408(undefined1 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  short sVar2;
  byte local_c;
  
  sVar2 = 0x4408;
  FUN_00075c3d(0x4408,param_1);
  if (((local_c & 1) != 0) || (uVar1 = 1, sVar2 != 1)) {
    uVar1 = 0;
  }
  return CONCAT44(param_2,uVar1);
}

