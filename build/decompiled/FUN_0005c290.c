
/* WARNING: Removing unreachable block (ram,0x0005c2d2) */

int __regparm3 FUN_0005c290(int param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  int extraout_ECX;
  
  if (0 < param_2) {
    param_1 = param_2;
  }
  if (param_1 == 0) {
    return 0;
  }
  iVar1 = FUN_0007521f();
  if (iVar1 == 0) {
    uVar2 = FUN_0005cdf0();
    FUN_00049660(&DAT_000a92d4,uVar2);
    FUN_00049620();
  }
  iVar3 = FUN_0005c320();
  if (iVar3 != 0) {
    FUN_0005bad0();
  }
  piVar4 = (int *)FUN_0005c320();
  if (piVar4 != (int *)0x0) {
    *piVar4 = iVar1;
    piVar4[1] = extraout_ECX;
    piVar4[2] = (int)DAT_000cf2c8;
  }
  DAT_000cf2c8 = piVar4;
  return iVar1;
}

 (GhidraScript)  