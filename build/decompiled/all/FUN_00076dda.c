
undefined4 __regparm3
FUN_00076dda(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined1 param_4)

{
  undefined4 unaff_EBX;
  undefined4 unaff_EBP;
  undefined4 unaff_ESI;
  undefined4 unaff_EDI;
  undefined2 in_ES;
  undefined2 in_CS;
  undefined2 in_SS;
  undefined2 in_DS;
  undefined2 in_FS;
  undefined2 in_GS;
  undefined4 unaff_retaddr;
  
  *param_1 = unaff_EBX;
  param_1[1] = param_3;
  param_1[2] = param_2;
  param_1[3] = unaff_ESI;
  param_1[4] = unaff_EDI;
  param_1[5] = unaff_EBP;
  param_1[6] = unaff_retaddr;
  param_1[7] = &param_4;
  *(undefined2 *)(param_1 + 8) = in_ES;
  *(undefined2 *)((int)param_1 + 0x22) = in_DS;
  *(undefined2 *)(param_1 + 9) = in_CS;
  *(undefined2 *)((int)param_1 + 0x26) = in_FS;
  *(undefined2 *)(param_1 + 10) = in_GS;
  *(undefined2 *)((int)param_1 + 0x2a) = in_SS;
  return 0;
}

