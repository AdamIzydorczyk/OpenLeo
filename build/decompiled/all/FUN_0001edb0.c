
void FUN_0001edb0(void)

{
  int iVar1;
  undefined4 in_ECX;
  int extraout_ECX;
  int extraout_ECX_00;
  
  FUN_00040030(in_ECX);
  if (*(int *)(extraout_ECX + 0x5c) == 3) {
    iVar1 = *(int *)(*(int *)(*(int *)(extraout_ECX + 0x50) + 0xc) + 4);
    if (iVar1 == 0) {
      *(undefined4 *)(extraout_ECX + 0x124) = 0;
      return;
    }
    if ((iVar1 == 6) && (*(int *)(extraout_ECX + 0x124) == 0)) {
      FUN_0001ece0();
      *(int *)(extraout_ECX_00 + 0x124) = *(int *)(extraout_ECX_00 + 0x124) + 1;
    }
  }
  return;
}

