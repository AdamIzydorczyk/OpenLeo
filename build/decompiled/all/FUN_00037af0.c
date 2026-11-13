
void FUN_00037af0(void)

{
  int iVar2;
  int iVar1;
  
  iVar2 = 0;
  do {
    iVar1 = iVar2 + 4;
    *(undefined4 *)((int)&DAT_000cedf4 + iVar2) = 0;
    iVar2 = iVar1;
  } while (iVar1 != 0x1c);
  return;
}

