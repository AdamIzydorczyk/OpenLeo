
void __regparm3 FUN_000588a0(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int extraout_ECX;
  int extraout_EDX;
  
  do {
    uVar1 = FUN_00076bcc(param_2,param_3);
    *(undefined1 *)(extraout_EDX + -1) = uVar1;
  } while (extraout_EDX != param_1 + 0x1a);
  *(undefined4 *)(extraout_ECX + 0xe) = 0;
  return;
}

