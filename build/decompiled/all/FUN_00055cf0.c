
void __regparm3 FUN_00055cf0(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 *puVar2;
  undefined8 uVar3;
  
  FUN_00055d20(param_2);
  iVar1 = FUN_0001ceb0();
  if (iVar1 != 0) {
    uVar3 = FUN_00012b68();
    puVar2 = (undefined4 *)((ulonglong)uVar3 >> 0x20);
    *puVar2 = (int)uVar3;
    puVar2[1] = (int)uVar3;
    uVar3 = FUN_00012b74();
    *(int *)((int)((ulonglong)uVar3 >> 0x20) + 8) = (int)uVar3;
  }
  return;
}

