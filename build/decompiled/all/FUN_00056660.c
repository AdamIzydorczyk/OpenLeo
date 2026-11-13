
void FUN_00056660(void)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined8 uVar4;
  
  uVar4 = FUN_00056510();
  iVar3 = (int)((ulonglong)uVar4 >> 0x20);
  iVar2 = (int)uVar4;
  *(undefined4 *)(iVar2 + 0x14) = *(undefined4 *)(iVar3 + 0x14);
  *(undefined4 *)(iVar2 + 0x18) = *(undefined4 *)(iVar3 + 0x18);
  uVar1 = *(undefined4 *)(iVar3 + 0x1c);
  *(undefined4 *)(iVar2 + 0x20) = 0xffffffff;
  *(undefined ***)(iVar2 + 8) = &PTR_LAB_000a26c8;
  *(undefined4 *)(iVar2 + 0xc) = 0xff;
  *(undefined4 *)(iVar2 + 0x10) = 0x80;
  *(undefined4 *)(iVar2 + 0x1c) = uVar1;
  return;
}

