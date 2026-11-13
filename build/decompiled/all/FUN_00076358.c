
void FUN_00076358(void)

{
  undefined1 uVar1;
  undefined1 *extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  undefined1 *extraout_EDX;
  
  FUN_0007a4de();
  *extraout_ECX = 0x74;
  do {
    uVar1 = FUN_0007634c();
    *extraout_EDX = uVar1;
  } while (extraout_EDX + -1 != extraout_ECX);
  *(undefined1 *)(extraout_ECX_00 + 5) = 0x5f;
  uVar1 = FUN_0007634c();
  *(undefined1 *)(extraout_ECX_01 + 6) = uVar1;
  uVar1 = FUN_0007634c();
  *(undefined1 *)(extraout_ECX_02 + 8) = 0x2e;
  *(undefined1 *)(extraout_ECX_02 + 9) = 0x74;
  *(undefined1 *)(extraout_ECX_02 + 10) = 0x6d;
  *(undefined1 *)(extraout_ECX_02 + 0xb) = 0x70;
  *(undefined1 *)(extraout_ECX_02 + 0xc) = 0;
  *(undefined1 *)(extraout_ECX_02 + 7) = uVar1;
  return;
}

