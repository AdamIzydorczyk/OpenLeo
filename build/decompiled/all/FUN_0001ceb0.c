
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __regparm3 FUN_0001ceb0(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  ushort uVar1;
  ushort uVar2;
  int iVar3;
  uint uVar4;
  uint extraout_EDX;
  
  _DAT_000a71ac = 20000;
  DAT_000a71ae = 0x2000;
  _DAT_000ae672 = 3;
  iVar3 = FUN_000129b4(param_2,param_3);
  uVar1 = _DAT_000a71ac;
  if (iVar3 != 0) {
    uVar4 = (uint)_DAT_000a71ac;
    FUN_00012a64();
    _DAT_000ae672 = (undefined2)extraout_EDX;
    uVar4 = ((((extraout_EDX & 1) != 0) + 1) * uVar4 * (((extraout_EDX & 2) != 0) + 1)) / 0xc;
    uVar2 = (ushort)uVar4;
    uVar4 = uVar4 & 0xffff;
    _DAT_000a71ac = uVar1;
    if (uVar4 == 0x400 || uVar2 < 0x400) {
      DAT_000a71ae = 0x400;
    }
    else {
      if (uVar4 == 0x800 || uVar2 < 0x800) {
        DAT_000a71ae = 0x800;
        FUN_000129b4(param_2,param_3);
        return;
      }
      if (uVar4 == 0x1000 || uVar2 < 0x1000) {
        DAT_000a71ae = 0x1000;
        FUN_000129b4(param_2,param_3);
        return;
      }
      if (uVar4 == 0x2000 || uVar2 < 0x2000) {
        DAT_000a71ae = 0x2000;
        FUN_000129b4(param_2,param_3);
        return;
      }
      if (uVar4 == 0x4000 || uVar2 < 0x4000) {
        DAT_000a71ae = 0x4000;
        FUN_000129b4(param_2,param_3);
        return;
      }
      DAT_000a71ae = 0x8000;
    }
    FUN_000129b4(param_2,param_3);
  }
  return;
}

