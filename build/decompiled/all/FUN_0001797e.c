
void __regparm3 FUN_0001797e(byte *param_1,int param_2)

{
  undefined4 uVar1;
  
  *param_1 = *param_1 & 0xfb;
  *(undefined4 *)(param_1 + 0x39) = *(undefined4 *)(param_1 + 0x3d);
  uVar1 = *(undefined4 *)(param_1 + 0x49);
  *(undefined4 *)(param_1 + 0x3d) = uVar1;
  out((short)*(undefined4 *)(param_1 + 0x1c),(char)*(undefined4 *)(param_1 + 8));
  out((short)*(undefined4 *)(param_1 + 0x24),0);
  out((short)*(undefined4 *)(param_1 + 0x14),(char)*(undefined4 *)(param_1 + 0x45));
  out((short)*(undefined4 *)(param_1 + 0x14),(char)((uint)*(undefined4 *)(param_1 + 0x45) >> 8));
  out((short)*(undefined4 *)(param_1 + 0x10),(char)*(undefined4 *)(param_1 + 0x41));
  out((short)*(undefined4 *)(param_1 + 0x20),param_1[0x30]);
  out((short)*(undefined4 *)(param_1 + 0x24),0);
  out((short)*(undefined4 *)(param_1 + 0x18),(char)uVar1);
  out((short)*(undefined4 *)(param_1 + 0x18),(char)((uint)uVar1 >> 8));
  out((short)*(undefined4 *)(param_1 + 0x1c),(char)*(undefined4 *)(param_1 + 0xc));
  if (param_2 == 0) {
    FUN_00017933();
    return;
  }
  FUN_00017962();
  return;
}

