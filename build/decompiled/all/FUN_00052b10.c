
void __regparm3 FUN_00052b10(int param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 extraout_ECX;
  undefined4 unaff_EBX;
  undefined8 uVar4;
  
  piVar1 = (int *)(param_1 + 0x10);
  iVar2 = *piVar1;
  while (iVar2 != 0) {
    piVar1 = (int *)*piVar1;
    iVar2 = *piVar1;
  }
  uVar4 = FUN_0005c320();
  puVar3 = (undefined4 *)uVar4;
  if (puVar3 != (undefined4 *)0x0) {
    *puVar3 = 0;
    puVar3[1] = param_2;
    puVar3[2] = unaff_EBX;
    puVar3[3] = extraout_ECX;
  }
  *(undefined4 *)((ulonglong)uVar4 >> 0x20) = puVar3;
  return;
}

