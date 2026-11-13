
void __regparm3 FUN_0001c020(int param_1)

{
  int iVar1;
  int iVar2;
  
  if (*(uint *)(param_1 + 0xb0) < *(uint *)(param_1 + 0xac)) {
    do {
      if (*(int *)(param_1 + 0x94) != 0) {
        FUN_0001bee0();
      }
      if ((undefined1 *)(*(int *)(param_1 + 0xac) - *(int *)(param_1 + 0xb0)) < &LAB_00010001) {
        iVar2 = FUN_0007612f();
        iVar1 = *(int *)(param_1 + 0xac);
        *(int *)(param_1 + 0x148) = *(int *)(param_1 + 0x148) + iVar2;
        *(undefined4 *)(param_1 + 0xac) = 0;
        *(int *)(param_1 + 0xa8) = *(int *)(param_1 + 0xa8) + (iVar1 - *(int *)(param_1 + 0xb0));
        *(undefined4 *)(param_1 + 0xb0) = *(undefined4 *)(param_1 + 0xac);
      }
      else {
        FUN_0007612f();
        *(short *)(param_1 + 0xb2) = *(short *)(param_1 + 0xb2) + 1;
        *(short *)(param_1 + 0xaa) = *(short *)(param_1 + 0xaa) + 1;
        *(short *)(param_1 + 0x14a) = *(short *)(param_1 + 0x14a) + 1;
      }
    } while (*(uint *)(param_1 + 0xb0) < *(uint *)(param_1 + 0xac));
  }
  return;
}

