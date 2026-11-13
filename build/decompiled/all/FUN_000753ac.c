
int __regparm3 FUN_000753ac(int param_1,int param_2)

{
  uint *puVar1;
  int iVar2;
  int extraout_ECX;
  int *unaff_EBX;
  int *piVar3;
  int iVar4;
  uint uVar5;
  int local_18;
  
  puVar1 = *(uint **)(param_1 + 4);
  piVar3 = unaff_EBX + param_2;
  local_18 = 0;
  while( true ) {
    if (piVar3 <= unaff_EBX) {
      return 0;
    }
    if (*unaff_EBX == 0) break;
    iVar4 = 0;
    for (uVar5 = 0; uVar5 < *puVar1; uVar5 = uVar5 + 1) {
      *(int *)(param_1 + 0x14) = (int)puVar1 + iVar4 + 4;
      iVar2 = FUN_00079461();
      if (iVar2 != 0) {
        return local_18 + 1;
      }
      iVar4 = iVar4 + 8;
      param_1 = extraout_ECX;
    }
    unaff_EBX = unaff_EBX + 1;
    local_18 = local_18 + 1;
  }
  return local_18 + 1;
}

