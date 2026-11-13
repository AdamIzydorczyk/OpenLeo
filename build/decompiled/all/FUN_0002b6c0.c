
void FUN_0002b6c0(void)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  
  iVar2 = FUN_00057500();
  iVar3 = FUN_00057530();
  (**(code **)(*(int *)(extraout_ECX + 8) + 0x68))();
  FUN_0004b3b0();
  FUN_0004a460();
  FUN_0004a030();
  if ((((local_3c < iVar3) || (iVar3 + 200 < local_44)) || (local_40 < iVar2)) ||
     (iVar2 + 0x140 < local_48)) {
    bVar1 = false;
  }
  else {
    bVar1 = true;
  }
  if (bVar1) {
    (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x68))();
    FUN_0004b3c0();
    *(undefined4 *)(extraout_ECX_01 + 0x2c) = 1;
  }
  else {
    *(undefined4 *)(extraout_ECX_00 + 0x2c) = 0;
  }
  FUN_0002b520();
  return;
}

