
void FUN_00059f80(void)

{
  int iVar1;
  undefined4 *extraout_ECX;
  
  iVar1 = FUN_00057dd0();
  if (iVar1 == -1) {
    if (*(char *)*extraout_ECX == '\\') {
      FUN_00057bb0();
    }
    else {
      FUN_00057a70();
      FUN_00057bb0();
      FUN_00057a00();
    }
  }
  FUN_00057c90();
  return;
}

