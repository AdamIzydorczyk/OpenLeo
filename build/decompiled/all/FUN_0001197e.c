
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int __regparm3
FUN_0001197e(undefined4 param_1,int param_2,int param_3,undefined4 param_4,undefined4 param_5,
            byte param_6)

{
  int iVar1;
  int extraout_ECX;
  int extraout_ECX_00;
  int extraout_ECX_01;
  int local_c;
  int local_8;
  
  FUN_00012728(param_3);
  local_8 = 0;
  for (iVar1 = 0; (iVar1 < 0x200 && (*(int *)((int)&DAT_000adebc + iVar1) != 0)); iVar1 = iVar1 + 4)
  {
    local_8 = local_8 + 1;
  }
  if (local_8 == 0x80) {
    _DAT_000c3fb0 = PTR_s_Out_of_sample_handles_000a7cdc;
  }
  else {
    local_c = param_2;
    if ((param_6 & 1) != 0) {
      local_c = param_2 * 2;
      param_3 = param_3 * 2;
    }
    if ((param_6 & 1) == 0) {
      iVar1 = FUN_00010f70(param_3);
    }
    else {
      iVar1 = FUN_00011004();
    }
    (&DAT_000adebc)[local_8] = iVar1;
    if (iVar1 != 0) {
      *(int *)(&DAT_000ae0bc + local_8 * 4) = local_c + 8;
      for (; 0 < local_c; local_c = local_c - extraout_ECX) {
        FUN_0001274c();
        FUN_00010824();
      }
      if (((param_6 & 0x10) == 0) || ((param_6 & 0x20) != 0)) {
        do {
          FUN_00010754();
        } while (extraout_ECX_01 < 8);
      }
      else {
        do {
          FUN_00010714();
          FUN_00010754();
        } while (extraout_ECX_00 < 8);
      }
      return local_8;
    }
    _DAT_000c3fb0 = PTR_s_Sample_too_big__out_of_memory_000a7ce0;
  }
  return -1;
}

