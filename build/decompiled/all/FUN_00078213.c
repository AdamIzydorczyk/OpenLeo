
int __regparm3 FUN_00078213(int param_1,int *param_2)

{
  byte bVar1;
  undefined2 uVar2;
  undefined2 *extraout_ECX;
  int iVar3;
  int extraout_EDX;
  int iVar4;
  undefined1 local_18;
  
  if ((*(byte *)(param_1 + 0x10) & 1) != 0) {
    if ((*(byte *)(param_1 + 0x10) & 4) == 0) {
      if ((*(byte *)(param_1 + 0x10) & 8) == 0) {
        *param_2 = *param_2 + 4;
      }
      else {
        *param_2 = *param_2 + 4;
      }
    }
    else {
      *param_2 = *param_2 + 8;
    }
  }
  iVar3 = *(int *)(param_1 + 0xc);
  iVar4 = 0;
  if (iVar3 == -1) {
    iVar3 = 1;
  }
  while( true ) {
    if (iVar3 < 1) {
      return iVar4;
    }
    uVar2 = FUN_00077e89();
    bVar1 = *(byte *)(param_1 + 0x10);
    if ((bVar1 & 2) != 0) break;
    iVar4 = iVar4 + 1;
    iVar3 = extraout_EDX + -1;
    if ((bVar1 & 1) != 0) {
      if ((bVar1 & 0x20) == 0) {
        local_18 = (undefined1)uVar2;
        *(undefined1 *)extraout_ECX = local_18;
      }
      else {
        *extraout_ECX = uVar2;
      }
    }
  }
  return iVar4;
}

