
undefined8 __regparm3 FUN_00016a30(undefined4 param_1,undefined4 param_2)

{
  undefined1 uVar1;
  undefined1 uVar2;
  undefined4 extraout_EDX;
  undefined4 unaff_EBX;
  
  uVar1 = FUN_00016a18();
  uVar2 = FUN_00016a18();
  return CONCAT44(param_2,(CONCAT31((int3)((uint)unaff_EBX >> 8),uVar1) & 0xffff00ff) << 8 |
                          CONCAT31((int3)((uint)extraout_EDX >> 8),uVar2));
}

