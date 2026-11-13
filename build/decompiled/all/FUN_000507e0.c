
void __regparm3 FUN_000507e0(int param_1,undefined4 param_2)

{
  undefined1 uVar1;
  int extraout_ECX;
  int unaff_EBX;
  int iVar2;
  int in_GS_OFFSET;
  
  switch(param_2) {
  case 1:
    DAT_000a94f0 = DAT_000a94f0 + 1;
    if (9 < DAT_000a94f0) {
      DAT_000a94f0 = 9;
      return;
    }
    break;
  case 2:
    DAT_000a94f0 = DAT_000a94f0 + -1;
    if (DAT_000a94f0 < 1) {
      DAT_000a94f0 = 1;
      return;
    }
    break;
  case 3:
    iVar2 = *(int *)(param_1 + 0xad) + 0x32;
    *(int *)(param_1 + 0xad) = iVar2;
    if (100 < iVar2) {
      *(undefined4 *)(param_1 + 0xad) = 100;
      return;
    }
    break;
  case 4:
    *(int *)(param_1 + 0xad) = *(int *)(param_1 + 0xad) + -0x1e;
    return;
  case 5:
    *(undefined4 *)(param_1 + 0x11a) = 5;
    *(int *)(param_1 + 0x11e) = (int)(0xaf / (longlong)unaff_EBX);
    return;
  case 6:
    *(undefined4 *)(param_1 + 0x11a) = 6;
    *(undefined1 *)(param_1 + 0xb5) = 1;
    *(int *)(param_1 + 0x11e) = (int)(0x15e / (longlong)unaff_EBX);
    return;
  case 7:
    *(undefined4 *)(param_1 + 0x11a) = 7;
    *(int *)(param_1 + 0x11e) = (int)(0x15e / (longlong)unaff_EBX);
    uVar1 = (**(code **)(*(int *)(param_1 + 8) + 0xdc))();
    *(undefined4 *)(extraout_ECX + 0xb1) = 1000;
    *(undefined1 *)(extraout_ECX + 0xb5) = uVar1;
    return;
  case 8:
    *(undefined4 *)(param_1 + 0x11a) = 8;
    return;
  case 9:
    FUN_00050eb0();
    return;
  case 10:
    *(undefined4 *)(&DAT_000c0d1d + in_GS_OFFSET) = 0xc;
    return;
  case 0xb:
    return;
  case 0xc:
    *(undefined4 *)(param_1 + 0xad) = 0xffffffff;
  }
  return;
}

