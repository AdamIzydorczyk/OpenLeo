
undefined4 __regparm3 FUN_0003a170(int param_1,undefined4 param_2)

{
  int iVar1;
  int unaff_EBX;
  int *piVar2;
  undefined4 local_14;
  undefined4 local_10;
  
  local_14 = 0x3a183;
  local_10 = param_2;
  iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x3c))();
  if (iVar1 != 0) {
    local_14 = 0x3a193;
    iVar1 = (**(code **)(*(int *)(unaff_EBX + 8) + 0x68))();
    local_14 = 0x3a19f;
    iVar1 = (**(code **)(*(int *)(iVar1 + 8) + 8))();
    if (iVar1 != 0) {
      piVar2 = &local_10;
      if ((*(byte *)(*(int *)(DAT_000cef40 + 0x394) + 100) & 0x80) != 0) {
        if (*(int *)(*(int *)(DAT_000cef40 + 0x394) + 0xee) == *(int *)(param_1 + 0xf2)) {
          local_14 = 0x3a1ec;
          FUN_00052b10();
          *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x40;
          piVar2 = &local_10;
          if (*(int *)(param_1 + 0xfa) != 0) {
            local_14 = 0;
            FUN_0004bc40();
            piVar2 = &local_14;
          }
        }
        else {
          *(byte *)(param_1 + 0x66) = *(byte *)(param_1 + 0x66) | 0x80;
          piVar2 = &local_10;
        }
      }
      *(undefined1 *)(*piVar2 + 0xc) = 0;
      piVar2[-1] = 0x3a22d;
      FUN_000749a0();
      return *piVar2;
    }
  }
  local_14 = 0x3a241;
  FUN_000552e0();
  return local_10;
}

