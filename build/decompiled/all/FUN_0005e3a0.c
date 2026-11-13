
void __regparm3 FUN_0005e3a0(int param_1)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  
  *(undefined4 *)(param_1 + 0x58) = 0;
  *(int *)(param_1 + 4) = *(int *)(param_1 + 0x58);
  iVar3 = 1;
  piVar1 = (int *)(param_1 + 4);
  do {
    iVar2 = iVar3 + 0xb;
    piVar1[0x16] = iVar2;
    iVar3 = iVar3 + 1;
    piVar1[1] = iVar2;
    piVar1 = piVar1 + 1;
  } while (iVar3 < 0x15);
  iVar3 = 0;
  do {
    iVar2 = iVar3 + 4;
    *(undefined4 *)((int)&DAT_000cf46c + iVar3) = 0;
    iVar3 = iVar2;
  } while (iVar2 != 0x54);
  DAT_000cf464 = 0;
  DAT_000a94f0 = 5;
  return;
}

