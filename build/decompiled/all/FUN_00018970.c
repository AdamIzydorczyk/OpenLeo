
undefined8 __regparm3 FUN_00018970(undefined4 param_1,undefined4 param_2)

{
  uint uVar1;
  undefined4 uVar2;
  ushort extraout_CX;
  ushort extraout_CX_00;
  
  DAT_000c3fdc = FUN_0001a5d0();
  if (DAT_000c3fdc == 0) {
    uVar2 = 0;
  }
  else {
    while( true ) {
      uVar1 = FUN_0001a600();
      if ((undefined1 *)((uVar1 >> 1 & 0xffff) + (uint)extraout_CX) < &LAB_00010001) break;
      DAT_000c3fdc = DAT_000c3fdc + 1;
    }
    FUN_000749a0();
    DAT_000c3fe8 = FUN_0001a5d0();
    uVar2 = 0;
    if (DAT_000c3fe8 != 0) {
      while( true ) {
        uVar1 = FUN_0001a600();
        if (((undefined1 *)((uVar1 >> 1 & 0xffff) + (uint)extraout_CX_00) < &LAB_00010001) &&
           ((DAT_000c3fe8 & 0x1f) == 0)) break;
        DAT_000c3fe8 = DAT_000c3fe8 + 1;
      }
      FUN_000749a0();
      uVar2 = 1;
    }
  }
  return CONCAT44(param_2,uVar2);
}

