
void __regparm3 FUN_0004a2e0(byte *param_1,undefined4 param_2,undefined4 param_3)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  undefined1 local_30c [768];
  undefined4 uStack_c;
  undefined4 uStack_8;
  
  iVar2 = 0;
  do {
    bVar1 = *param_1;
    iVar3 = iVar2 + 1;
    param_1 = param_1 + 1;
    local_30c[iVar2] = (char)((int)(uint)bVar1 >> 2);
    iVar2 = iVar3;
  } while (iVar3 < 0x300);
  local_30c[2] = 0;
  local_30c[1] = 0;
  local_30c[0] = 0;
  uStack_c = param_2;
  uStack_8 = param_3;
  FUN_0004a2c0();
  return;
}

