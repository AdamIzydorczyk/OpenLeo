
ulonglong __regparm3 FUN_000178a2(undefined1 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined2 uVar2;
  
  out((short)DAT_000a887b,param_1);
  uVar2 = (undefined2)DAT_000a887f;
  out(uVar2,0x8a);
  in((short)DAT_000a8883);
  out(uVar2,0x8b);
  in((short)DAT_000a8883);
  out(uVar2,0x80);
  in((short)DAT_000a8887);
  uVar1 = FUN_00017fd7();
  return CONCAT44(param_2,uVar1) & 0xffffffff000fffff;
}

