
void __regparm3 FUN_00075d1f(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int extraout_EDX;
  
  iVar1 = DAT_000cf5b4;
  if (DAT_000cf5b4 != 0x21) {
    DAT_000cf5b4 = 0x21;
    iVar1 = iVar1 << 2;
    while (iVar1 != 0) {
      (**(code **)((int)&DAT_000cf530 + iVar1))(*(code **)((int)&DAT_000cf530 + iVar1),param_2);
      iVar1 = extraout_EDX;
    }
  }
  return;
}

