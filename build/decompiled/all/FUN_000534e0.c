
void __regparm3 FUN_000534e0(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  undefined8 uVar3;
  
  uVar3 = (**(code **)(*(int *)(param_1 + 8) + 0x68))();
  *(undefined4 *)((ulonglong)uVar3 >> 0x20) = *(undefined4 *)uVar3;
  uVar3 = (**(code **)(*(int *)(param_1 + 8) + 0x68))();
  *(undefined4 *)((int)((ulonglong)uVar3 >> 0x20) + 4) = *(undefined4 *)((int)uVar3 + 4);
  iVar2 = (**(code **)(*(int *)(param_1 + 8) + 0x48))();
  puVar1 = *(undefined4 **)(iVar2 + 0x33);
  (**(code **)(*(int *)(param_1 + 8) + 0x50))();
  (*(code *)*puVar1)();
  return;
}

