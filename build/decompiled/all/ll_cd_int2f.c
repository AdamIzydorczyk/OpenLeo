
/* INT2F AX=1500/150B real-CD query */

longlong __regparm3 ll_cd_int2f(undefined4 param_1,uint param_2)

{
  undefined4 uVar1;
  char extraout_CL;
  short sVar2;
  short sVar3;
  
  sVar3 = 0;
  FUN_00075c3d(0x1500);
  if (sVar3 == 0) {
    uVar1 = 0;
  }
  else {
    sVar2 = 0x150b;
    FUN_00075c3d(0x150b,sVar3,extraout_CL + -1);
    if ((sVar3 != -0x5253) || (sVar2 == 0)) {
      return (ulonglong)param_2 << 0x20;
    }
    uVar1 = 1;
  }
  return CONCAT44(param_2,uVar1);
}

