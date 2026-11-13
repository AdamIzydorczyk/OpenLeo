
void FUN_00056620(void)

{
  int iVar1;
  undefined8 uVar2;
  
  uVar2 = FUN_00056510();
  iVar1 = (int)uVar2;
  *(undefined4 *)(iVar1 + 0x18) = 0;
  *(undefined4 *)(iVar1 + 0x1c) = 0;
  *(undefined4 *)(iVar1 + 0x20) = 0xffffffff;
  *(undefined4 *)(iVar1 + 0x24) = 0;
  *(undefined ***)(iVar1 + 8) = &PTR_LAB_000a26c8;
  *(undefined4 *)(iVar1 + 0xc) = 0xff;
  *(undefined4 *)(iVar1 + 0x10) = 0x80;
  *(int *)(iVar1 + 0x14) = (int)((ulonglong)uVar2 >> 0x20);
  return;
}

