
void __regparm3 FUN_0007961b(int param_1)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar2 = *(undefined4 **)(param_1 + 8);
  puVar3 = puVar2 + -1;
  *(undefined4 *)(param_1 + 8) = *puVar2;
  *puVar2 = *puVar3;
  puVar2 = (undefined4 *)(puVar2[4] + 4);
  while ((puVar1 = (undefined4 *)*puVar2, puVar1 != (undefined4 *)0x0 && (puVar1 <= puVar3))) {
    puVar2 = puVar1;
  }
  *puVar2 = puVar3;
  *puVar3 = puVar1;
  return;
}

