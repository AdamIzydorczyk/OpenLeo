
void __regparm3 FUN_000796f0(undefined4 param_1,undefined4 *param_2)

{
  undefined4 extraout_ECX;
  undefined2 *unaff_EBX;
  undefined4 unaff_ESI;
  undefined4 unaff_EDI;
  undefined2 in_ES;
  undefined2 in_DS;
  byte in_CF;
  undefined8 uVar1;
  
  uVar1 = FUN_00079728();
  *param_2 = (int)uVar1;
  param_2[1] = unaff_EBX;
  param_2[2] = extraout_ECX;
  param_2[3] = (int)((ulonglong)uVar1 >> 0x20);
  param_2[4] = unaff_ESI;
  param_2[5] = unaff_EDI;
  param_2[6] = -(uint)in_CF;
  unaff_EBX[3] = in_DS;
  *unaff_EBX = in_ES;
  return;
}

