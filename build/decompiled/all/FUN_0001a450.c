
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm3 FUN_0001a450(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  if (DAT_000c4064 == 0) {
    if ((DAT_000c40b0 & 1) == 0) {
      _DAT_000c40f4 = FUN_0001a5d0(param_2,param_3);
      while( true ) {
        if ((undefined1 *)((uint)DAT_000c40b6 + (_DAT_000c40f4 & 0xffff)) < &LAB_00010001) break;
        _DAT_000c40f4 = _DAT_000c40f4 + 1;
      }
      DAT_000c40f8 = (undefined1)(_DAT_000c40f4 >> 0x10);
      DAT_000c40fa = DAT_000c40f4;
    }
    else {
      _DAT_000c40f4 = FUN_0001a5d0(param_2,param_3);
      while( true ) {
        if ((undefined1 *)((_DAT_000c40f4 >> 1 & 0xffff) + (uint)DAT_000c40b6) < &LAB_00010001)
        break;
        _DAT_000c40f4 = _DAT_000c40f4 + 1;
      }
      DAT_000c40fa = (undefined2)(_DAT_000c40f4 >> 1);
      DAT_000c40f8 = (undefined1)(_DAT_000c40f4 >> 0x10);
    }
    DAT_000c40e4 = _DAT_000c40f4;
    FUN_000749a0();
    DAT_000c407c = DAT_000c40e4;
    DAT_000c4080 = DAT_000c40e4 + ((int)(uint)DAT_000c40b6 >> 1);
    FUN_0001a060();
  }
  else {
    FUN_00018c40();
  }
  return;
}

