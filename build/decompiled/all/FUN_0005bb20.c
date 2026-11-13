
int * FUN_0005bb20(void)

{
  int *piVar1;
  int iVar2;
  uint in_ECX;
  undefined4 extraout_ECX;
  undefined4 uVar3;
  uint unaff_EBX;
  undefined4 local_18;
  uint local_14;
  undefined4 *local_10;
  
  piVar1 = (int *)FUN_0001c790();
  for (; unaff_EBX != 0; unaff_EBX = unaff_EBX - local_14) {
    if (in_ECX < unaff_EBX) {
      local_14 = in_ECX;
    }
    else {
      local_14 = unaff_EBX;
    }
    iVar2 = FUN_0005c320();
    uVar3 = 0;
    if (iVar2 != 0) {
      FUN_0005bad0();
      uVar3 = extraout_ECX;
    }
    local_10 = &local_18;
    local_18 = uVar3;
    if (piVar1[2] == piVar1[3]) {
      FUN_0001c7b0();
    }
    iVar2 = piVar1[2];
    piVar1[2] = iVar2 + 1;
    *(undefined4 *)(*piVar1 + iVar2 * 4) = *local_10;
  }
  return piVar1;
}

