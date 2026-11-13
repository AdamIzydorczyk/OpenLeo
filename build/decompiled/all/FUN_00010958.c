
void __regparm3 FUN_00010958(int param_1)

{
  int extraout_ECX;
  int extraout_ECX_00;
  int iVar1;
  int iVar2;
  
  if (param_1 < 0xe) {
    param_1 = 0xe;
  }
  if (0x20 < param_1) {
    param_1 = 0x20;
  }
  DAT_000ade97 = 0;
  DAT_000ade98 = 0;
  DAT_000ade96 = (undefined1)param_1;
  FUN_000108c0();
  iVar1 = 0;
  FUN_000105dc();
  do {
    iVar1 = iVar1 + 1;
    FUN_00010690();
  } while (iVar1 < 10);
  FUN_000105dc();
  iVar1 = 0;
  do {
    iVar1 = iVar1 + 1;
    FUN_00010690();
  } while (iVar1 < 10);
  iVar1 = 0;
  out(DAT_000ade94 + 0x100,3);
  do {
    iVar1 = iVar1 + 1;
    FUN_00010690();
  } while (iVar1 < 10);
  out(DAT_000ade94 + 0x100,0);
  FUN_000105dc();
  FUN_000105dc();
  FUN_000105dc();
  FUN_000105dc();
  in(DAT_000ade94 + 6);
  FUN_00010638();
  FUN_00010638();
  FUN_00010638();
  iVar1 = extraout_ECX;
  for (iVar2 = 0; iVar2 < iVar1; iVar2 = iVar2 + 1) {
    out(DAT_000ade94 + 0x102,(char)iVar2);
    FUN_000105dc();
    FUN_000105dc();
    FUN_00010690();
    FUN_00010608();
    FUN_00010608();
    FUN_00010608();
    FUN_00010608();
    FUN_00010608();
    FUN_000105dc();
    FUN_000105dc();
    FUN_000105dc();
    FUN_00010608();
    FUN_00010608();
    FUN_00010608();
    FUN_000105dc();
    iVar1 = extraout_ECX_00;
  }
  in(DAT_000ade94 + 6);
  FUN_00010638();
  FUN_00010638();
  FUN_00010638();
  FUN_000105dc();
  return;
}

