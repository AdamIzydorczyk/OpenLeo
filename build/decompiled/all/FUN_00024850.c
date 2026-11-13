
void FUN_00024850(void)

{
  int iVar1;
  int extraout_ECX;
  
  iVar1 = FUN_0005c320();
  if (iVar1 == 0) {
    iVar1 = 0;
  }
  else {
    iVar1 = FUN_00054030();
    *(undefined ***)(iVar1 + 8) = &PTR_LAB_000983a4;
    *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009839c;
    *(undefined ***)(iVar1 + 8) = &PTR_LAB_00098724;
    *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009871c;
  }
  (**(code **)(*(int *)(iVar1 + 8) + 4))();
  (**(code **)(*(int *)(extraout_ECX + 8) + 0x80))();
  return;
}

