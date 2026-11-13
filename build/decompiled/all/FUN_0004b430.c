
int __regparm3 FUN_0004b430(int param_1,int param_2)

{
  int iVar1;
  byte *extraout_ECX;
  int unaff_EBX;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar3 = 0;
  iVar4 = 0;
  iVar5 = 0;
  iVar6 = 0;
  FUN_000534e0(0,0,0,0,&PTR_FUN_00098274);
  iVar3 = iVar3 + *(int *)(unaff_EBX + 0xca);
  iVar5 = iVar5 + *(int *)(unaff_EBX + 0xca);
  iVar4 = iVar4 + *(int *)(unaff_EBX + 0xce);
  iVar6 = iVar6 + *(int *)(unaff_EBX + 0xce);
  iVar2 = 0;
  if (0 < *(int *)(param_1 + 0x7c)) {
    do {
      iVar1 = FUN_00045620(iVar3,iVar4,iVar5,iVar6);
      if (iVar1 != 0) {
        *(undefined1 *)(param_2 + 0xc) = 0;
        FUN_000749a0();
        *extraout_ECX = *extraout_ECX | 1;
        return param_2;
      }
      iVar2 = iVar2 + 1;
    } while (iVar2 < *(int *)(param_1 + 0x7c));
  }
  *(undefined1 *)(param_2 + 0xc) = 0;
  FUN_000749a0();
  return param_2;
}

