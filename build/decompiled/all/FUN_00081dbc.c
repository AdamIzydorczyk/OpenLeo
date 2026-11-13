
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint FUN_00081dbc(int param_1,uint param_2,ushort param_3,undefined4 param_4,uint param_5,
                 uint param_6)

{
  longdouble lVar1;
  uint uVar2;
  longdouble lVar3;
  
  lVar1 = (longdouble)CONCAT28(param_3,CONCAT44(param_2,param_1));
  while (!CARRY4(param_2,param_2)) {
    if (param_1 == 0 && param_2 == 0) {
      return 0;
    }
    if ((param_3 & 0x7fff) != 0) {
      return param_3 & 0x7fff;
    }
    uVar2 = param_6 & 0x7fff;
    if (uVar2 == 0) {
      if (CARRY4(param_5,param_5)) {
        return param_5 * 2;
      }
    }
    else if ((uVar2 == 0x7fff) || (uVar2 = param_5 * 2, !CARRY4(param_5,param_5))) {
      return uVar2;
    }
    lVar3 = lVar1 * (longdouble)_DAT_000ad968;
    param_1 = SUB104(lVar3,0);
    param_3 = (ushort)((unkuint10)lVar3 >> 0x40);
    param_2 = (uint)((unkuint10)lVar3 >> 0x20);
  }
  uVar2 = param_2 * 2 ^ 0xe000000;
  if ((uVar2 & 0xe000000) != 0) {
    return uVar2;
  }
  uVar2 = (param_2 & 0x7fffffff) >> 0x1b;
  if ((&DAT_000ad950)[uVar2] == '\0') {
    return uVar2;
  }
  if (((param_3 & 0x7fff) == 0) || ((param_3 & 0x7fff) == 0x7fff)) {
    return param_3 & 0x7fff;
  }
  if ((param_6 & 0x7fff) == 1) {
    return 1;
  }
  return param_6 & 0x7fff;
}

