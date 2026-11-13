
void __regparm3 FUN_0003d200(int param_1)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int extraout_ECX_03;
  undefined8 uVar2;
  int iVar3;
  
  if (((*(byte *)(param_1 + 0x62) & 0x80) != 0) && (*(int *)(param_1 + 0x120) != 0)) {
    (**(code **)(*(int *)(*(int *)(param_1 + 0x120) + 8) + 0x68))();
    iVar3 = *(int *)(*(int *)(extraout_ECX + 0x120) + 8);
    uVar2 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x68))();
    iVar1 = *(int *)((int)uVar2 + 4) + *(int *)((int)((ulonglong)uVar2 >> 0x20) + 4);
    uVar2 = (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x70))();
    (**(code **)(*(int *)(extraout_ECX_01 + 8) + 0x68))
              (iVar3,*(int *)((ulonglong)uVar2 >> 0x20) * (int)uVar2,iVar1);
    (**(code **)(iVar3 + 4))();
    (**(code **)(*(int *)(extraout_ECX_02 + 8) + 0x80))();
    *(undefined4 *)(extraout_ECX_03 + 0x120) = 0;
  }
  FUN_00040030();
  return;
}

