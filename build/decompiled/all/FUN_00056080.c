
void __regparm3 FUN_00056080(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 unaff_EBX;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  
  *(undefined4 *)(param_1 + 0x28) = 1;
  iVar3 = param_2 * 0x28;
  if (param_2 < *(int *)(param_1 + 0x20)) {
    iVar1 = 0;
    if (0 < *(int *)(param_1 + 0x3c)) {
      do {
        puVar5 = (undefined4 *)(&DAT_000b33d0 + (*(int *)(param_1 + 0x20) + iVar1) * 0x28);
        puVar6 = (undefined4 *)(&DAT_000b33d0 + iVar3);
        for (iVar2 = 10; iVar2 != 0; iVar2 = iVar2 + -1) {
          *puVar6 = *puVar5;
          puVar5 = puVar5 + 1;
          puVar6 = puVar6 + 1;
        }
        iVar1 = iVar1 + 1;
        iVar3 = iVar3 + 0x28;
      } while (iVar1 < *(int *)(param_1 + 0x3c));
    }
  }
  else if ((*(int *)(param_1 + 0x20) < param_2) &&
          (iVar1 = *(int *)(param_1 + 0x3c) + -1, -1 < iVar1)) {
    iVar3 = iVar1 * 0x28 + iVar3;
    do {
      iVar4 = *(int *)(param_1 + 0x20) + iVar1;
      puVar5 = (undefined4 *)(&DAT_000b33d0 + iVar3);
      iVar3 = iVar3 + -0x28;
      iVar1 = iVar1 + -1;
      puVar6 = (undefined4 *)(&DAT_000b33d0 + iVar4 * 0x28);
      for (iVar2 = 10; iVar2 != 0; iVar2 = iVar2 + -1) {
        *puVar5 = *puVar6;
        puVar6 = puVar6 + 1;
        puVar5 = puVar5 + 1;
      }
    } while (-1 < iVar1);
  }
  *(int *)(param_1 + 0x20) = param_2;
  *(undefined4 *)(param_1 + 0x24) = unaff_EBX;
  DAT_000ae674 = *(char *)(param_1 + 0x20) + *(char *)(param_1 + 0x24);
  *(undefined4 *)(param_1 + 0x28) = 0;
  return;
}

