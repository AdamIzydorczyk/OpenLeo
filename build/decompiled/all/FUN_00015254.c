
void __regparm3 FUN_00015254(byte param_1,undefined4 param_2)

{
  uint uVar1;
  uint extraout_EDX;
  
  uVar1 = (uint)*(ushort *)((int)&PTR_LAB_000a7a3c + (uint)param_1 * 2);
  while ((short)uVar1 != 0) {
    FUN_00015230(param_2);
    uVar1 = extraout_EDX;
  }
  return;
}

