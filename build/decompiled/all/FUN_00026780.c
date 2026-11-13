
undefined4 __regparm3 FUN_00026780(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int *extraout_ECX;
  uint uVar2;
  uint uVar3;
  int *unaff_EBX;
  
  iVar1 = (**(code **)(unaff_EBX[2] + 0x3c))();
  if ((iVar1 != 0) &&
     (uVar2 = *extraout_ECX - *unaff_EBX >> 0x1f, uVar3 = extraout_ECX[1] - unaff_EBX[1] >> 0x1f,
     (int)(((extraout_ECX[1] - unaff_EBX[1] ^ uVar3) - uVar3) +
          ((*extraout_ECX - *unaff_EBX ^ uVar2) - uVar2)) < extraout_ECX[0x48])) {
    *(undefined4 *)((int)extraout_ECX + 0xad) = 0xffffffff;
  }
  FUN_000552e0();
  return param_2;
}

