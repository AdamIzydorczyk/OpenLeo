
undefined8 __regparm3 FUN_000132b0(byte *param_1,undefined4 param_2)

{
  byte bVar1;
  ushort uVar2;
  int iVar3;
  undefined4 uVar4;
  
  bVar1 = *param_1;
  if (bVar1 != 0) {
    iVar3 = FUN_0001317c();
    *(int *)(param_1 + 0xd5) = iVar3;
    uVar4 = 0;
    if (iVar3 == 0) goto LAB_00013307;
    for (uVar2 = 0; uVar2 < bVar1; uVar2 = uVar2 + 1) {
      *(undefined1 *)((uint)uVar2 * 0x1d + 4 + *(int *)(param_1 + 0xd5)) = 0x80;
      *(undefined2 *)((uint)uVar2 * 0x1d + 0x1b + *(int *)(param_1 + 0xd5)) = 0xffff;
    }
  }
  uVar4 = 1;
LAB_00013307:
  return CONCAT44(param_2,uVar4);
}

