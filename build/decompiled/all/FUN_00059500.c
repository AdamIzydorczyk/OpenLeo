
undefined4 __regparm3 FUN_00059500(int param_1,int param_2)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  undefined8 uVar3;
  
  if (param_2 == 0) {
    return 0;
  }
  while ((*(int *)(param_1 + 0x14) <= *(int *)(param_1 + 0x18) && (*(int *)(param_1 + 0x1c) != 0)))
  {
    FUN_000594a0();
  }
  uVar3 = FUN_0005c320();
  puVar2 = (undefined4 *)uVar3;
  if (puVar2 != (undefined4 *)0x0) {
    uVar1 = *(undefined4 *)(param_1 + 0x1c);
    *puVar2 = (int)((ulonglong)uVar3 >> 0x20);
    puVar2[1] = uVar1;
  }
  *(undefined4 **)(param_1 + 0x1c) = puVar2;
  *(int *)(param_1 + 0x18) = *(int *)(param_1 + 0x18) + 1;
  return **(undefined4 **)(param_1 + 0x1c);
}

