
undefined8 __regparm3 FUN_00052a00(undefined4 param_1,undefined4 param_2)

{
  int *piVar1;
  int *extraout_ECX;
  uint uVar2;
  uint uVar3;
  
  FUN_00052d30();
  piVar1 = (int *)FUN_00052d30();
  uVar2 = *piVar1 - *extraout_ECX >> 0x1f;
  uVar3 = piVar1[1] - extraout_ECX[1] >> 0x1f;
  return CONCAT44(param_2,((piVar1[1] - extraout_ECX[1] ^ uVar3) - uVar3) +
                          ((*piVar1 - *extraout_ECX ^ uVar2) - uVar2));
}

