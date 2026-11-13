
void __regparm3 FUN_00013510(int param_1)

{
  int extraout_EDX;
  int extraout_EDX_00;
  int extraout_EDX_01;
  int extraout_EDX_02;
  int extraout_EDX_03;
  int extraout_EDX_04;
  int extraout_EDX_05;
  int extraout_EDX_06;
  ushort uVar1;
  
  if (*(int *)(param_1 + 0x132) != 0) {
    FUN_00075302();
    param_1 = extraout_EDX;
  }
  if (*(int *)(param_1 + 0x13e) != 0) {
    FUN_0001319c();
    param_1 = extraout_EDX_00;
  }
  if (*(int *)(param_1 + 0x142) != 0) {
    FUN_0001319c();
    param_1 = extraout_EDX_01;
  }
  if (*(int *)(param_1 + 0x146) != 0) {
    for (uVar1 = 0; uVar1 < *(ushort *)(param_1 + 7); uVar1 = uVar1 + 1) {
      if (*(int *)(*(int *)(param_1 + 0x146) + (uint)uVar1 * 4) != 0) {
        FUN_0001319c();
        param_1 = extraout_EDX_02;
      }
    }
    FUN_0001319c();
    param_1 = extraout_EDX_03;
  }
  if (*(int *)(param_1 + 0x13a) != 0) {
    for (uVar1 = 0; uVar1 < *(ushort *)(param_1 + 9); uVar1 = uVar1 + 1) {
      FUN_000134b4();
      param_1 = extraout_EDX_04;
    }
    FUN_0001319c();
    param_1 = extraout_EDX_05;
  }
  if (*(int *)(param_1 + 0x12e) != 0) {
    FUN_00075302();
    param_1 = extraout_EDX_06;
  }
  if (*(int *)(param_1 + 0x136) != 0) {
    FUN_00075302();
  }
  return;
}

