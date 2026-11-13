
void __regparm3 FUN_0001f4e0(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar2;
  int iVar1;
  
  iVar2 = 0;
  do {
    iVar1 = iVar2 + 4;
    *(undefined4 *)((int)&DAT_000cebec + iVar2) = *(undefined4 *)((int)&DAT_000cebf0 + iVar2);
    iVar2 = iVar1;
  } while (iVar1 != 8);
  DAT_000cebf4 = *(undefined4 *)(param_1 + 0x122);
  if (*(int *)(param_1 + 0x5c) != 6) {
    FUN_0001f330(param_3);
  }
  *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x40;
  FUN_0003df90();
  return;
}

