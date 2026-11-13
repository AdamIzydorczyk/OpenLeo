
bool __regparm3 FUN_000812e8(undefined1 param_1,undefined4 param_2)

{
  ushort uVar1;
  longdouble lVar2;
  bool bVar3;
  longdouble in_ST0;
  
  lVar2 = (longdouble)0;
  uVar1 = (ushort)(in_ST0 == lVar2);
  bVar3 = in_ST0 >= lVar2;
  if (bVar3 && uVar1 == 0) {
    log2(in_ST0);
  }
  else {
    FUN_00081ba0((double)in_ST0,
                 CONCAT11(in_ST0 < lVar2 |
                          (byte)((ushort)((ushort)(NAN(in_ST0) || NAN(lVar2)) << 10) >> 8) |
                          (byte)((ushort)(uVar1 << 0xe) >> 8),param_1),param_2);
  }
  return !bVar3 || uVar1 != 0;
}

