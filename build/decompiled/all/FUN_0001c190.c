
void FUN_0001c190(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = 0;
  do {
    iVar1 = iVar2 + 4;
    iVar3 = 0;
    *(undefined4 *)((int)&DAT_000ce284 + iVar2) = 0;
    iVar2 = iVar1;
  } while (iVar1 != 0x800);
  iVar2 = 0;
  do {
    if (*(int *)((int)&DAT_000a89e4 + iVar3) != 0xff) {
      (&DAT_000ce284)[*(int *)((int)&DAT_000a89e4 + iVar3)] = iVar2;
    }
    if (*(int *)((int)&DAT_000a8be4 + iVar3) != 0xff) {
      (&DAT_000ce284)[*(int *)((int)&DAT_000a8be4 + iVar3)] = iVar2;
    }
    iVar2 = iVar2 + 1;
    iVar3 = iVar3 + 4;
  } while (iVar2 < 0x80);
  return;
}

