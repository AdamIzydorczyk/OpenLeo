
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_00027c50(void)

{
  int iVar1;
  undefined8 uVar2;
  
  uVar2 = FUN_00076bcc();
  iVar1 = (int)((ulonglong)uVar2 >> 0x20);
  *(uint *)(iVar1 + 0x19a) =
       (uint)((double)(int)uVar2 / _DAT_000909e7 < (double)*(float *)(iVar1 + 0x19e));
  return;
}

