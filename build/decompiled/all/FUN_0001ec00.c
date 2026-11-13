
/* WARNING: Removing unreachable block (ram,0x0001ec7a) */

int __regparm3 FUN_0001ec00(int param_1,uint param_2)

{
  if ((param_2 & 4) == 0) {
    *(undefined ***)(param_1 + 8) = &PTR_FUN_000978dc;
    *(undefined ***)(param_1 + 0xc) = &PTR_LAB_000978d4;
    FUN_0004bc40(0);
    FUN_0004bc40();
    param_1 = FUN_00055770();
  }
  else {
    FUN_00074bc0();
    thunk_FUN_0005c370();
  }
  return param_1;
}

