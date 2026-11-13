
undefined8 __regparm2
FUN_00081253(undefined4 param_1,undefined4 param_2,undefined4 param_3,uint param_4,
            undefined4 *param_5)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  short local_10;
  short sStack_e;
  short local_c;
  
  local_c = (short)param_4;
  if ((param_4 >> 0x10 & 0x7ff0) == 0x7ff0) {
    puVar1 = param_5;
    if ((param_4 & 0x80000000) != 0) {
      puVar1 = (undefined4 *)((int)param_5 + 1);
      *(undefined1 *)param_5 = 0x2d;
    }
    sStack_e = (short)((uint)param_3 >> 0x10);
    local_10 = (short)param_3;
    if ((local_c == 0 && (sStack_e == 0 && local_10 == 0)) && (param_4 & 0xf0000) == 0) {
      uVar2 = DAT_000960b4;
    }
    else {
      uVar2 = DAT_000960b8;
    }
    *puVar1 = uVar2;
    uVar2 = 1;
  }
  else {
    uVar2 = 0;
  }
  return CONCAT44(param_2,uVar2);
}

