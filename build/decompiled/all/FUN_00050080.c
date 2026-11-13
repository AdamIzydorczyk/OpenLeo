
void FUN_00050080(void)

{
  int iVar2;
  int iVar1;
  
  iVar2 = 0;
  do {
    iVar1 = iVar2 + 4;
    *(undefined4 *)((int)&DAT_000cef8c + iVar2) = 0;
    *(undefined4 *)((int)&DAT_000cef5c + iVar2) = 0;
    iVar2 = iVar1;
  } while (iVar1 != 0x30);
  return;
}

