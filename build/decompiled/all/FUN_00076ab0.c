
undefined4 __regparm3 FUN_00076ab0(int param_1,uint param_2)

{
  code *pcVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  int unaff_EBX;
  uint local_28;
  undefined4 local_24;
  
  uVar3 = 0;
  if (param_1 != 0) {
    puVar2 = (undefined4 *)FUN_0007b791();
    pcVar1 = *(code **)(unaff_EBX + 1);
    for (; local_28 < param_2; local_28 = local_28 + 1) {
      (*pcVar1)();
    }
    *puVar2 = *(undefined4 *)*puVar2;
    uVar3 = local_24;
  }
  return uVar3;
}

