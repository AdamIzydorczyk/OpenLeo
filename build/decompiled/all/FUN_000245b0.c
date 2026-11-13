
void __regparm3 FUN_000245b0(int *param_1,undefined4 *param_2)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 *unaff_EBX;
  undefined4 **ppuVar4;
  int iVar5;
  int iVar6;
  undefined4 local_24;
  undefined4 *local_20;
  
  ppuVar4 = &local_20;
  if (((0x640 < *param_1) && (uVar1 = param_1[0x17], 9 < uVar1)) &&
     ((uVar1 < 0xf || (uVar1 == 0x11)))) {
    *param_2 = 1;
    *unaff_EBX = 0;
  }
  local_20 = param_2;
  if (((*(int *)((int)param_1 + 0x12e) == 0) && (*(int *)((int)param_1 + 0x12a) != 0)) &&
     (*(int *)((int)param_1 + 0x13e) != 0)) {
    iVar5 = 0;
    *(undefined4 *)(*(int *)(DAT_000cef40 + 0x394) + 0x106) = 0;
    for (iVar6 = 0; iVar6 < *(int *)((int)param_1 + 0x13e); iVar6 = iVar6 + 1) {
      uVar2 = *(undefined4 *)(iVar5 + 4 + *(int *)((int)param_1 + 0x136));
      local_24 = 0x24669;
      iVar3 = FUN_0005c320();
      if (iVar3 != 0) {
        local_24 = 0x2467d;
        iVar3 = FUN_00053eb0();
        *(undefined4 *)(iVar3 + 4) = 4;
        *(undefined4 *)(iVar3 + 0x1d) = uVar2;
      }
      iVar5 = iVar5 + 0xc;
    }
    *(undefined4 *)((int)param_1 + 0x12e) = 1;
  }
  if (*(int *)((int)param_1 + 0x132) != 0) {
    *local_20 = 0x16;
    local_24 = 0;
    *unaff_EBX = 0;
    *(undefined4 *)((int)param_1 + 0x132) = 0;
    FUN_0004bc40();
    ppuVar4 = (undefined4 **)&local_24;
  }
  *(undefined4 *)((int)ppuVar4 + -4) = 0x246f2;
  FUN_00050970();
  return;
}

