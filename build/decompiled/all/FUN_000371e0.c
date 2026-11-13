
void __regparm3 FUN_000371e0(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  (&DAT_000ced1c)[param_1 * 3] = (&DAT_000ced1c)[param_2 * 3];
  (&DAT_000ced18)[param_2 * 3] = (&DAT_000ced18)[param_1 * 3];
  if (DAT_000cedf0 != 0) {
    iVar4 = DAT_000cedf0;
    do {
      iVar1 = iVar4 + param_1 * 0x14;
      if (*(int *)(iVar1 + 0x134) == -1) {
        iVar2 = iVar4 + param_2 * 0x14;
        iVar3 = *(int *)(iVar2 + 0x134);
        if (iVar3 != -1) {
          *(int *)(iVar1 + 0x134) = iVar3;
          *(undefined4 *)(iVar1 + 0x13c) = *(undefined4 *)(iVar2 + 0x13c);
        }
      }
      iVar1 = iVar4 + param_2 * 0x14;
      if (*(int *)(iVar1 + 0x134) == -1) {
        iVar2 = iVar4 + param_1 * 0x14;
        iVar3 = *(int *)(iVar2 + 0x134);
        if (iVar3 != -1) {
          *(int *)(iVar1 + 0x134) = iVar3;
          *(undefined4 *)(iVar1 + 0x13c) = *(undefined4 *)(iVar2 + 0x13c);
        }
      }
      iVar4 = *(int *)(iVar4 + 0x288);
    } while (iVar4 != 0);
  }
  return;
}

