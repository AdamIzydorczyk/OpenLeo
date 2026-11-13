
void __regparm3 FUN_0007c9a4(byte param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  byte *unaff_EBP;
  byte *pbVar2;
  undefined4 *puVar3;
  undefined4 auStack_80 [29];
  undefined4 uStack_c;
  
  unaff_EBP[4] = unaff_EBP[4] | param_1;
  if ((*unaff_EBP & param_1) == 0) {
    pbVar2 = unaff_EBP;
    puVar3 = auStack_80;
    uStack_c = param_3;
    for (iVar1 = 0x1b; iVar1 != 0; iVar1 = iVar1 + -1) {
      *puVar3 = *(undefined4 *)pbVar2;
      pbVar2 = pbVar2 + 4;
      puVar3 = puVar3 + 1;
    }
    (*(code *)PTR_FUN_000a9bf9)();
    puVar3 = auStack_80;
    for (iVar1 = 0x1b; iVar1 != 0; iVar1 = iVar1 + -1) {
      *(undefined4 *)unaff_EBP = *puVar3;
      puVar3 = puVar3 + 1;
      unaff_EBP = unaff_EBP + 4;
    }
  }
  return;
}

