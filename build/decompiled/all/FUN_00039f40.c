
void __regparm3 FUN_00039f40(undefined4 param_1,undefined4 param_2)

{
  undefined8 uVar1;
  
  uVar1 = FUN_00076bcc(param_2);
  DAT_000cee20 = (DAT_000cee18 + (uint)((int)uVar1 * (int)((ulonglong)uVar1 >> 0x20)) / 0x7fff) *
                 DAT_000cee24;
  return;
}

