
void FUN_00030070(void)

{
  undefined4 uVar1;
  int iVar2;
  int extraout_ECX;
  int extraout_ECX_00;
  
  iVar2 = FUN_0005c320();
  if (iVar2 == 0) {
    iVar2 = 0;
  }
  else {
    iVar2 = FUN_00054030();
    *(undefined4 *)(iVar2 + 0xee) = *(undefined4 *)(extraout_ECX + 0xee);
    *(undefined4 *)(iVar2 + 0xf2) = *(undefined4 *)(extraout_ECX + 0xf2);
    *(undefined4 *)(iVar2 + 0xf6) = *(undefined4 *)(extraout_ECX + 0xf6);
    uVar1 = *(undefined4 *)(extraout_ECX + 0xfa);
    *(undefined ***)(iVar2 + 8) = &PTR_LAB_0009c05c;
    *(undefined ***)(iVar2 + 0xc) = &PTR_LAB_0009c054;
    *(undefined4 *)(iVar2 + 0xfa) = uVar1;
  }
  *(undefined4 *)(iVar2 + 0x5c) = 1;
  *(undefined4 *)(*(int *)(*(int *)(iVar2 + 0x50) + 4) + 4) = 0;
  (**(code **)(*(int *)(iVar2 + 8) + 4))();
  (**(code **)(*(int *)(iVar2 + 8) + 0xd8))();
  *(undefined4 *)(iVar2 + 0xad) = *(undefined4 *)(extraout_ECX_00 + 0xfa);
  (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x80))();
  FUN_000544d0();
  return;
}

