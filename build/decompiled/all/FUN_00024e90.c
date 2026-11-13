
int __regparm3 FUN_00024e90(undefined4 param_1,int param_2)

{
  int iVar1;
  int *extraout_ECX;
  uint uVar2;
  uint uVar3;
  int *unaff_EBX;
  
  iVar1 = (**(code **)(unaff_EBX[2] + 0x3c))();
  if ((iVar1 == 0) ||
     (uVar2 = *extraout_ECX - *unaff_EBX >> 0x1f, uVar3 = extraout_ECX[1] - unaff_EBX[1] >> 0x1f,
     0x27 < (int)(((extraout_ECX[1] - unaff_EBX[1] ^ uVar3) - uVar3) +
                 ((*extraout_ECX - *unaff_EBX ^ uVar2) - uVar2)))) {
    FUN_000552e0();
  }
  else {
    if ((*(byte *)(*(int *)(DAT_000cef40 + 0x394) + 100) & 0x80) != 0) {
      if (*(int *)(*(int *)(DAT_000cef40 + 0x394) + 0xee) == 1) {
        FUN_00052b10();
      }
      else {
        *(byte *)((int)extraout_ECX + 0x66) = *(byte *)((int)extraout_ECX + 0x66) | 0x40;
      }
    }
    *(undefined1 *)(param_2 + 0xc) = 0;
    FUN_000749a0();
  }
  return param_2;
}

