
int FUN_00017c38(int param_1)

{
  int iVar1;
  byte in_CL;
  int iVar2;
  uint unaff_EBX;
  
  iVar2 = DAT_000a8822 - 1;
  iVar1 = FUN_00017a21();
  if (iVar1 == 1) {
    if (3 < DAT_000a8822) {
      unaff_EBX = FUN_0001800f();
    }
    out((short)DAT_000a887f,0x42);
    out((short)DAT_000a8883,(short)(unaff_EBX >> 4));
    *(byte *)(iVar2 * 0x4e + 0xa85f5) = in_CL & 0xfe;
    DAT_000a881a = DAT_000a881a | 2;
    FUN_00017933();
    if (param_1 == 0) {
      DAT_000a881a = DAT_000a881a | 8;
    }
    else {
      FUN_00017c24();
    }
    iVar1 = 1;
  }
  return iVar1;
}

