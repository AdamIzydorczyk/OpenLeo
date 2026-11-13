
void __regparm3 FUN_0001c3d0(uint *param_1)

{
  undefined6 uVar1;
  
  FUN_0001c490();
  *param_1 = 1;
  uVar1 = FUN_00075c5a();
  *(undefined6 *)(param_1 + 1) = uVar1;
  FUN_00075c8c();
  DAT_000cde55 = 2;
  FUN_00075c3d();
  if ((DAT_000cde54 & 0x20) != 0) {
    *param_1 = *param_1 | 2;
  }
  FUN_0001c190();
  return;
}

