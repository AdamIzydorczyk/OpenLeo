
void FUN_00036d90(void)

{
  int iVar2;
  int iVar1;
  
  iVar2 = 0;
  do {
    iVar1 = iVar2 + 0xc;
    *(undefined4 *)((int)&DAT_000ced18 + iVar2) = *(undefined4 *)((int)&DAT_000a8f94 + iVar2);
    *(undefined4 *)((int)&DAT_000ced1c + iVar2) = *(undefined4 *)((int)&DAT_000a8f98 + iVar2);
    *(undefined4 *)((int)&DAT_000ced20 + iVar2) = *(undefined4 *)((int)&DAT_000a8f9c + iVar2);
    iVar2 = iVar1;
  } while (iVar1 != 0xd8);
  DAT_000cedf0 = 0;
  return;
}

