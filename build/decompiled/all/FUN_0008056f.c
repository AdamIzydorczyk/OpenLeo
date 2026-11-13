
void __regparm3 FUN_0008056f(undefined1 *param_1,int param_2)

{
  char cVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined1 *puVar4;
  int unaff_EBX;
  int iVar5;
  
  if (param_2 != unaff_EBX) {
    cVar1 = param_1[1];
    puVar4 = param_1 + unaff_EBX;
    puVar2 = param_1 + param_2;
    do {
      iVar5 = unaff_EBX;
      puVar2 = puVar2 + -1;
      puVar4 = puVar4 + -1;
      *puVar4 = *puVar2;
      unaff_EBX = iVar5 + -1;
    } while (puVar2 != param_1);
    iVar3 = CONCAT31((uint3)((uint)puVar2 >> 8) ^ (uint3)((uint)param_1 >> 8),*param_1);
    if (iVar3 == 0x2e) {
      param_1[iVar5 + -2] = 0x30;
    }
    else if (((iVar3 == 0x2b) || (iVar3 == 0x2d)) && (cVar1 == '.')) {
      param_1[iVar5 + -1] = 0x30;
      param_1[iVar5 + -2] = *param_1;
    }
    FUN_000749a0();
  }
  return;
}

