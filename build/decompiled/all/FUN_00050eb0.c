
void FUN_00050eb0(void)

{
  int iVar2;
  int iVar1;
  
  iVar2 = 0;
  do {
    iVar1 = iVar2 + 4;
    *(undefined4 *)((int)&DAT_000cef5c + iVar2) = 1;
    iVar2 = iVar1;
  } while (iVar1 != 0x30);
  return;
}

