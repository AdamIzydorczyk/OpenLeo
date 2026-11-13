
void __regparm3 FUN_0002b860(int param_1)

{
  int iVar1;
  int extraout_ECX;
  undefined8 uVar2;
  
  iVar1 = (**(code **)(*(int *)(param_1 + 8) + 0xb0))(0,0,&PTR_LAB_00098278);
  uVar2 = (**(code **)(*(int *)(iVar1 + 8) + 0x68))();
  uVar2 = (**(code **)(*(int *)((int)((ulonglong)uVar2 >> 0x20) + 8) + 0x48))
                    (*(undefined4 *)uVar2,((undefined4 *)uVar2)[1]);
  iVar1 = *(int *)((int)uVar2 + 0x33);
  (**(code **)(*(int *)((int)((ulonglong)uVar2 >> 0x20) + 8) + 0x50))();
  (**(code **)(iVar1 + 0xc))();
  (**(code **)(*(int *)(extraout_ECX + 8) + 4))();
  FUN_0002b6c0();
  return;
}

