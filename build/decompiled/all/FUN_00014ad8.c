
undefined4 __regparm3 FUN_00014ad8(byte *param_1,undefined4 param_2)

{
  byte bVar1;
  ushort uVar2;
  byte *extraout_ECX;
  char unaff_BL;
  ushort uVar3;
  uint uVar4;
  byte local_1c;
  byte local_18;
  
  if ((*param_1 & 1) != 0) {
    local_1c = param_1[7];
    uVar2 = *(ushort *)(param_1 + 5);
    bVar1 = param_1[9];
    param_2 = FUN_00014a54();
    local_18 = bVar1;
    if (((((*extraout_ECX & 2) == 0) || (unaff_BL == '\0')) ||
        ((uint)extraout_ECX[2] != (uint)local_1c)) ||
       (uVar3 = uVar2,
       (uint)uVar2 != (int)*(short *)((uint)local_1c * 4 + *(int *)(extraout_ECX + 0xb)))) {
      uVar4 = uVar2 + 1;
      uVar3 = (ushort)uVar4;
      if ((int)*(short *)(*(int *)(extraout_ECX + 0xb) + (uint)bVar1 * 4) <= (int)(uVar4 & 0xffff))
      {
        local_18 = bVar1 + 1;
        local_1c = bVar1;
        if ((*extraout_ECX & 4) == 0) {
          if (extraout_ECX[1] <= local_18) {
            uVar3 = uVar2;
            local_18 = bVar1;
          }
        }
        else if (extraout_ECX[4] < local_18) {
          local_1c = extraout_ECX[3];
          uVar3 = *(ushort *)(*(int *)(extraout_ECX + 0xb) + (uint)local_1c * 4);
          local_18 = local_1c + 1;
        }
      }
    }
    *(ushort *)(extraout_ECX + 5) = uVar3;
    *(ushort *)(extraout_ECX + 7) = (ushort)local_1c;
    *(ushort *)(extraout_ECX + 9) = (ushort)local_18;
  }
  return param_2;
}

