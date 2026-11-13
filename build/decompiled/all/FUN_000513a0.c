
undefined4 __regparm3 FUN_000513a0(undefined4 param_1)

{
  int iVar1;
  
  FUN_0001c490();
  iVar1 = 0;
  do {
    if (*(int *)((int)&DAT_000cefbc + iVar1) != 0) {
      (*(code *)**(undefined4 **)(*(int *)((int)&DAT_000cefbc + iVar1) + 4))();
    }
    iVar1 = iVar1 + 4;
  } while (iVar1 != 0x38);
  return param_1;
}

