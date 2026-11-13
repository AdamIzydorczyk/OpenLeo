
void __regparm3 FUN_00050d60(int param_1,int param_2)

{
  int extraout_ECX;
  int iVar1;
  
  if ((*(int *)(param_1 + 0x126) == 0) || (*(int *)(param_2 + 4) != 1)) {
    switch(*(undefined4 *)(param_2 + 4)) {
    case 9:
      if (*(int *)(param_1 + 0xad) < 100) {
        FUN_000749a0();
        FUN_00049580();
        iVar1 = *(int *)(extraout_ECX + 0xad) + 0x20;
        *(int *)(extraout_ECX + 0xad) = iVar1;
        if (100 < iVar1) {
          *(undefined4 *)(extraout_ECX + 0xad) = 100;
          return;
        }
      }
      break;
    case 10:
      *(undefined4 *)(param_1 + 0x14) = *(undefined4 *)(param_2 + 8);
      *(undefined4 *)(param_1 + 0x18) = *(undefined4 *)(param_2 + 0xc);
      return;
    case 0xb:
      (**(code **)(*(int *)(param_1 + 8) + 0xd8))();
      return;
    default:
      FUN_00054560();
      break;
    case 0xf:
      (**(code **)(*(int *)(param_1 + 8) + 0xfc))();
      return;
    case 0x15:
      *(undefined4 *)(param_1 + 0x116) = 1;
      return;
    }
  }
  return;
}

