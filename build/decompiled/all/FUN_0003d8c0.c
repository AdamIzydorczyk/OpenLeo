
void FUN_0003d8c0(void)

{
  int iVar1;
  undefined8 uVar2;
  
  uVar2 = FUN_00053ff0();
  iVar1 = (int)uVar2;
  *(undefined4 *)(iVar1 + 0xee) = 0;
  *(undefined ***)(iVar1 + 8) = &PTR_LAB_00096874;
  *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009686c;
  *(int *)(iVar1 + 0xb1) = (int)((ulonglong)uVar2 >> 0x20);
  return;
}

