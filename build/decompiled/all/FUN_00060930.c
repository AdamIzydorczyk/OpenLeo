
int FUN_00060930(void)

{
  int iVar1;
  undefined8 uVar2;
  
  iVar1 = FUN_0005f3d0();
  *(undefined4 *)(iVar1 + 0xa0) = 0;
  *(undefined ***)(iVar1 + 0x9c) = &PTR_FUN_000a2d90;
  uVar2 = thunk_FUN_0005c320();
  iVar1 = (int)((ulonglong)uVar2 >> 0x20);
  *(int *)(iVar1 + 0xa0) = (int)uVar2;
  return iVar1;
}

