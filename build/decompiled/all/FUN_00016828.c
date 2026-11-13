
undefined8 __regparm3 FUN_00016828(int param_1,undefined4 param_2)

{
  undefined1 uVar1;
  
  uVar1 = (&DAT_000a7c44)[param_1 * 0x10];
  out(*(undefined2 *)(&DAT_000a7c4c + param_1 * 0x10),uVar1);
  return CONCAT44(param_2,CONCAT31((int3)((uint)(&DAT_000a7c44 + param_1 * 0x10) >> 8),uVar1));
}

