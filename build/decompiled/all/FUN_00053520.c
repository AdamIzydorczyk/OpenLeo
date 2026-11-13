
void __regparm3 FUN_00053520(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  undefined4 auStack_2c [3];
  undefined1 local_20;
  undefined4 *local_1c;
  undefined4 local_18;
  
  local_1c = (undefined4 *)(param_1 + 0x60);
  local_20 = *(undefined1 *)(param_1 + 0x6c);
  puVar2 = local_1c;
  puVar3 = auStack_2c;
  local_18 = param_2;
  for (iVar1 = 3; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  for (iVar1 = 0; iVar1 != 0; iVar1 = iVar1 + -1) {
    *(undefined1 *)puVar3 = *(undefined1 *)puVar2;
    puVar2 = (undefined4 *)((int)puVar2 + 1);
    puVar3 = (undefined4 *)((int)puVar3 + 1);
  }
  FUN_00052000();
  (**(code **)(*(int *)(param_1 + 8) + 0x90))();
  FUN_000749a0();
  return;
}

