
void FUN_00016328(void)

{
  int iVar1;
  uint uVar2;
  undefined1 uVar3;
  
  uVar2 = 0;
  iVar1 = 0;
  do {
    *(undefined4 *)((int)&DAT_000b33e6 + iVar1) = 10000;
    *(undefined4 *)((int)&DAT_000b33ec + iVar1) = 0;
    *(undefined2 *)((int)&DAT_000b33d2 + iVar1) = 0;
    *(undefined2 *)((int)&DAT_000b33d4 + iVar1) = 0;
    (&DAT_000b33d0)[iVar1] = 0;
    (&DAT_000b33d1)[iVar1] = 0;
    (&DAT_000b33ea)[iVar1] = 0;
    if ((uVar2 & 1) == 0) {
      uVar3 = 0xff;
    }
    else {
      uVar3 = 0;
    }
    uVar2 = uVar2 + 1;
    (&DAT_000b33eb)[iVar1] = uVar3;
    iVar1 = iVar1 + 0x28;
  } while ((int)uVar2 < 0x20);
  return;
}

