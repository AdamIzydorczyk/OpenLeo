
undefined4 FUN_00059940(void)

{
  bool bVar1;
  int iVar2;
  char *pcVar3;
  undefined4 uVar4;
  
  iVar2 = FUN_00010598();
  if (iVar2 != 0) {
    pcVar3 = (char *)FUN_00010598();
    if (*pcVar3 != '\0') {
      bVar1 = false;
      goto LAB_00059972;
    }
  }
  bVar1 = true;
LAB_00059972:
  if (bVar1) {
    FUN_00057990();
    FUN_000588d0();
    iVar2 = FUN_00059550();
    FUN_00057a00();
    FUN_00057a00();
    FUN_00057990();
    FUN_00058980();
    FUN_00057a00();
    if (iVar2 == 0) {
      uVar4 = 0xffffffff;
    }
    else {
      uVar4 = FUN_00058e90();
    }
    FUN_00057a00();
    return uVar4;
  }
  return 0xffffffff;
}

