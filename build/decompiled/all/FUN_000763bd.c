
uint __regparm3 FUN_000763bd(int param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int extraout_ECX_02;
  int iVar4;
  
  if (*(int *)(param_1 + 0xc) == 0) {
    uVar1 = 0xffffffff;
  }
  else {
    uVar1 = 0;
    if ((*(byte *)(param_1 + 0xd) & 0x10) != 0) {
      uVar1 = FUN_0007a4e4();
    }
    iVar2 = FUN_00076527();
    iVar4 = extraout_ECX;
    if (iVar2 != -1) {
      FUN_0007a5bd();
      iVar4 = extraout_ECX_00;
    }
    if (param_2 != 0) {
      uVar3 = FUN_0007a600();
      uVar1 = uVar1 | uVar3;
      iVar4 = extraout_ECX_01;
    }
    if ((*(byte *)(iVar4 + 0xc) & 8) != 0) {
      FUN_00075302();
      *(undefined4 *)(extraout_ECX_02 + 8) = 0;
      iVar4 = extraout_ECX_02;
    }
    if ((*(byte *)(iVar4 + 0xd) & 8) != 0) {
      FUN_00076358();
      thunk_FUN_0007bad0();
    }
  }
  return uVar1;
}

