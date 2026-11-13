
int __regparm3 FUN_000782a8(int param_1,int *param_2)

{
  byte bVar1;
  int iVar2;
  undefined2 *extraout_ECX;
  undefined2 *extraout_ECX_00;
  undefined2 *extraout_ECX_01;
  undefined2 *puVar3;
  int iVar4;
  int local_1c;
  char local_18;
  
  if ((*(byte *)(param_1 + 0x10) & 0x20) == 0) {
    local_18 = '\x01';
  }
  else {
    local_18 = '\x02';
  }
  bVar1 = *(byte *)(param_1 + 0x10);
  if ((bVar1 & 1) != 0) {
    if ((bVar1 & 4) == 0) {
      if ((bVar1 & 8) == 0) {
        *param_2 = *param_2 + 4;
      }
      else {
        *param_2 = *param_2 + 4;
      }
    }
    else {
      *param_2 = *param_2 + 8;
    }
  }
  iVar4 = 0;
  while( true ) {
    local_1c = FUN_00077e89();
    if (((&DAT_000a68b4)[(byte)((char)local_1c + 1)] & 2) == 0) break;
    iVar4 = iVar4 + 1;
  }
  if ((*(byte *)(param_1 + 0x10) & 2) == 0) {
    iVar2 = *(int *)(param_1 + 0xc);
    *(int *)(param_1 + 0xc) = iVar2 + -1;
    puVar3 = extraout_ECX;
    if (iVar2 != 0) {
      do {
        iVar4 = iVar4 + 1;
        if ((*(byte *)(param_1 + 0x10) & 1) != 0) {
          if (local_18 == '\x01') {
            *(char *)puVar3 = (char)local_1c;
          }
          else {
            *puVar3 = (short)local_1c;
          }
        }
        local_1c = FUN_00078a94();
        puVar3 = extraout_ECX_00;
        if (local_1c == -1) goto LAB_000783a0;
      } while (((&DAT_000a68b4)[(byte)((char)local_1c + 1)] & 2) == 0);
    }
    FUN_00077e90();
    puVar3 = extraout_ECX_01;
  }
  else {
    iVar4 = 0;
    puVar3 = extraout_ECX;
  }
LAB_000783a0:
  if (((*(byte *)(param_1 + 0x10) & 1) != 0) && (0 < iVar4)) {
    if (local_18 == '\x01') {
      *(undefined1 *)puVar3 = 0;
    }
    else {
      *puVar3 = 0;
    }
  }
  return iVar4;
}

