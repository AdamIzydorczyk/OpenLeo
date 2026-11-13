
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __regparm3 FUN_00016b25(uint param_1,uint *param_2)

{
  uint uVar1;
  
  if ((int)(uint)DAT_000a8863 <= (int)param_1) {
    return 8;
  }
  if (param_1 == 0xffffffff) {
    for (uVar1 = 0; param_1 = 99, uVar1 < DAT_000a8863; uVar1 = uVar1 + 1) {
      if ((_DAT_000a8865 & 1 << ((byte)uVar1 & 0x1f)) == 0) {
        param_1 = uVar1;
        break;
      }
    }
  }
  else if ((_DAT_000a8865 & 1 << ((byte)param_1 & 0x1f)) != 0) {
    return 9;
  }
  if ((int)param_1 < (int)(uint)DAT_000a8863) {
    *param_2 = param_1;
    _DAT_000a8865 = _DAT_000a8865 | 1 << ((byte)param_1 & 0x1f);
    return 1;
  }
  return 10;
}

