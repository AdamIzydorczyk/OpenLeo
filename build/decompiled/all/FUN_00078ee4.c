
undefined8 __regparm3 FUN_00078ee4(undefined4 param_1)

{
  code *pcVar1;
  int *piVar2;
  undefined4 *puVar3;
  undefined2 in_ES;
  byte bVar4;
  undefined8 uVar5;
  undefined4 uStack_18;
  undefined4 auStack_c [2];
  
  uVar5 = CONCAT44(DAT_000a9b58,param_1);
  puVar3 = auStack_c + -2;
  while (piVar2 = (int *)((ulonglong)uVar5 >> 0x20), piVar2 != (int *)0x0) {
    if (*(int *)piVar2[9] + 0x2c == *piVar2) {
      *(undefined4 *)((int)puVar3 + -4) = 0x78f12;
      FUN_00078e93();
      bVar4 = 0;
      if (piVar2[-1] == 0) {
        pcVar1 = (code *)swi(0x31);
        uVar5 = (*pcVar1)();
        puVar3 = (undefined4 *)((int)puVar3 + 4);
      }
      else {
        *(undefined2 *)((int)puVar3 + -4) = in_ES;
        pcVar1 = (code *)swi(0x21);
        uVar5 = (*pcVar1)();
        uVar5 = CONCAT44((int)((ulonglong)uVar5 >> 0x20),
                         ((int)uVar5 << 1 | (uint)bVar4) >> 1 | (uint)((bVar4 & 1) != 0) << 0x1f);
        in_ES = *(undefined2 *)puVar3;
        puVar3 = (undefined4 *)((int)puVar3 + 4);
      }
    }
    else {
      uVar5 = CONCAT44(piVar2[2],*(int *)piVar2[9] + 0x2c);
    }
  }
  return CONCAT44(*(undefined4 *)((int)puVar3 + 8),(int)uVar5);
}

