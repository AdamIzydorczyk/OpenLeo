
uint * __regparm3 FUN_00078f40(int *param_1,int param_2)

{
  undefined2 uVar1;
  code *pcVar2;
  int iVar3;
  undefined2 *puVar4;
  uint *puVar5;
  undefined2 extraout_CX;
  uint *extraout_ECX;
  int *extraout_EDX;
  uint uVar6;
  int extraout_EDX_00;
  int *piVar7;
  uint uVar8;
  char cVar9;
  
  FUN_00078ee4();
  piVar7 = DAT_000a9b58;
  while( true ) {
    if (piVar7 == (int *)0x0) {
      return (uint *)0x0;
    }
    if (param_1 < piVar7) {
      return (uint *)0x0;
    }
    if ((piVar7 + 0xb == param_1) && (piVar7[6] == 1)) break;
    piVar7 = (int *)piVar7[2];
  }
  FUN_00078e93();
  if (extraout_EDX[-1] != 0) {
    return (uint *)0x0;
  }
  uVar6 = *extraout_EDX + (param_2 - (*param_1 + -4)) + 0x100b;
  cVar9 = (uVar6 >> 8 & 0xff) >> 7 != 0;
  iVar3 = extraout_EDX[-2];
  uVar1 = *(undefined2 *)((int)extraout_EDX + -6);
  pcVar2 = (code *)swi(0x31);
  (*pcVar2)();
  puVar4 = (undefined2 *)0x0;
  if (cVar9 != '\x01') {
    puVar4 = (undefined2 *)CONCAT22((short)(uVar6 >> 0x10),extraout_CX);
    *puVar4 = (short)iVar3;
    puVar4[1] = uVar1;
  }
  if (puVar4 == (undefined2 *)0x0) {
    return (uint *)0x0;
  }
  *(undefined4 *)(puVar4 + 2) = 0;
  *(int *)(puVar4 + 4) = extraout_EDX_00 + -0xc;
  puVar5 = (uint *)FUN_0007903f();
  *(undefined4 *)(puVar4 + 0x10) = 1;
  uVar8 = param_2 + 1U & 0xfffffffe;
  uVar6 = *puVar5;
  if (uVar6 - uVar8 < 0xc) {
    *(byte *)puVar5 = (byte)*puVar5 | 1;
  }
  else {
    *puVar5 = uVar8 | 1;
    *(uint *)((int)puVar5 + uVar8) = uVar6 - uVar8 | 1;
    *(undefined4 *)(puVar4 + 0xe) = 0xffffffff;
    *(int *)(puVar4 + 0x10) = *(int *)(puVar4 + 0x10) + 1;
    FUN_00075302();
    puVar5 = extraout_ECX;
  }
  return puVar5;
}

