
void __regparm3 FUN_00041a50(int param_1,undefined4 *param_2)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  
  (**(code **)(*(int *)(param_1 + 8) + 0xe8))();
  param_2[1] = 0;
  param_2[2] = 0;
  *param_2 = 0;
  iVar1 = (**(code **)(*(int *)(extraout_ECX + 8) + 0x104))();
  if (iVar1 != 0) {
    if (*(float *)(extraout_ECX_00 + 0x10b) < *(float *)(extraout_ECX_00 + 0x12f)) {
      *(byte *)(extraout_ECX_00 + 0x61) = *(byte *)(extraout_ECX_00 + 0x61) | 1;
    }
    if ((*(char *)(extraout_ECX_00 + 0x160) == '\0') && (*(int *)(extraout_ECX_00 + 0x5c) == 1)) {
      if (*(char *)(extraout_ECX_00 + 0x106) == '\x01') {
        *(undefined1 *)(extraout_ECX_00 + 0x106) = 2;
      }
      else if (*(char *)(extraout_ECX_00 + 0x106) == '\x02') {
        *(undefined1 *)(extraout_ECX_00 + 0x106) = 1;
      }
    }
  }
  switch(*(undefined1 *)(extraout_ECX_00 + 0x160)) {
  case 0:
    (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0xf4))();
    FUN_00049580();
    return;
  case 1:
    (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0xf8))();
    FUN_00049580();
    return;
  case 2:
    (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0xfc))();
    FUN_00049580();
    return;
  case 3:
    (**(code **)(*(int *)(extraout_ECX_00 + 8) + 0x100))();
    FUN_00049580();
  }
  return;
}

