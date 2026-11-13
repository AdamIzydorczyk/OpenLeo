
int __regparm3 FUN_0001330c(int param_1,ushort param_2)

{
  ushort uVar1;
  undefined1 uVar2;
  int iVar3;
  undefined6 uVar4;
  
  iVar3 = 0;
  for (; (param_2 != 0 && (*(byte *)(param_1 + -1 + (uint)param_2) < 0x21)); param_2 = param_2 - 1)
  {
  }
  if ((param_2 != 0) && (uVar4 = crt_malloc(), iVar3 = (int)uVar4, iVar3 != 0)) {
    for (uVar1 = 0; uVar1 < (ushort)((uint6)uVar4 >> 0x20); uVar1 = uVar1 + 1) {
      if (*(byte *)((uint)uVar1 + param_1) < 0x20) {
        uVar2 = 0x20;
      }
      else {
        uVar2 = *(undefined1 *)((uint)uVar1 + param_1);
      }
      *(undefined1 *)(iVar3 + (uint)uVar1) = uVar2;
    }
    *(undefined1 *)(iVar3 + (uint)uVar1) = 0;
  }
  return iVar3;
}

