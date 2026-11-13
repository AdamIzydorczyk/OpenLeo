
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

longlong __regparm3 FUN_00018a60(uint param_1,uint param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  _DAT_000c3ff8 = 0;
  FUN_00018910();
  iVar1 = FUN_0001721d();
  if (iVar1 == 5) {
    uVar2 = 0;
  }
  else {
    iVar1 = FUN_000184cc();
    if (iVar1 == 5) {
      return (ulonglong)param_2 << 0x20;
    }
    iVar1 = FUN_00016b25();
    if (iVar1 != 1) {
      return (ulonglong)param_2 << 0x20;
    }
    _DAT_0003402c = (undefined2)(1 << ((byte)_DAT_000c3ff8 & 2) + ((byte)_DAT_000c3ff8 & 1));
    if ((DAT_00033ff8 & 2) == 0) {
      _DAT_00033fe0 = 0x80;
    }
    else {
      _DAT_00033fe0 = 0;
    }
    _DAT_00034088 = 0x13b << ((param_1 & 2) != 0) + ((param_1 & 1) != 0) & 0xffff;
    _DAT_00033ff0 = 0x140;
    _DAT_00034090 = 0;
    _DAT_0003408c = 0;
    _DAT_000c4094 = 0;
    _DAT_000c4098 = 0;
    if (((param_1 & 1) == 0) || ((param_1 & 2) == 0)) {
      if ((param_1 & 2) == 0) {
        if ((param_1 & 1) != 0) {
          _DAT_000c4098 = 1;
        }
      }
      else {
        _DAT_000c4094 = 1;
      }
    }
    else {
      _DAT_000c4090 = 1;
    }
    iVar1 = FUN_00018970();
    uVar2 = 0;
    if (iVar1 != 0) {
      iVar1 = FUN_00016fc1();
      if (iVar1 != 1) {
        return (ulonglong)param_2 << 0x20;
      }
      DAT_000c4024 = DAT_000c3ff0 + 0x20;
      DAT_000c4028 = DAT_000c4024 + DAT_000c3ff0 + -6;
      DAT_000c3ffc = 0x20;
      DAT_000c4020 = 0x25;
      FUN_0001790a();
      FUN_000177d4();
      FUN_0001706e();
      uVar2 = 1;
    }
  }
  return CONCAT44(param_2,uVar2);
}

