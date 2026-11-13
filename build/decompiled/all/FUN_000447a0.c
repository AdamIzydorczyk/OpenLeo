
void __regparm3 FUN_000447a0(int param_1)

{
  ulonglong uVar1;
  int iVar2;
  int extraout_ECX;
  
  uVar1 = CONCAT44(*(int *)(param_1 + 0x5c) >> 0x1f,*(undefined4 *)(param_1 + 0x5c)) / 10;
  if ((int)((longlong)((ulonglong)(uint)((int)uVar1 >> 0x1f) << 0x20 | uVar1 & 0xffffffff) % 2) == 0
     ) {
    iVar2 = *(int *)(*(int *)(*(int *)(param_1 + 0x78) + 8) +
                    *(int *)(*(int *)(DAT_000cef40 + 0x394) + 0xee) * 4);
  }
  else {
    iVar2 = *(int *)(*(int *)(*(int *)(param_1 + 0x78) + 8) + *(int *)(param_1 + 0x6c) * 4);
  }
  (**(code **)(*(int *)(iVar2 + 0x33) + 0x10))();
  if (*(int *)(extraout_ECX + 0x5c) != 0) {
    *(int *)(extraout_ECX + 0x5c) = *(int *)(extraout_ECX + 0x5c) + -1;
  }
  return;
}

