
undefined8 __regparm3 FUN_000817a7(uint param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  uint unaff_EDI;
  bool bVar3;
  
  uVar2 = param_1 | param_2;
  if (uVar2 != 0) {
    if ((param_2 & 0xfff00000) == 0) {
      do {
        unaff_EDI = unaff_EDI - 0x100000;
        bVar3 = CARRY4(param_1,param_1);
        param_1 = param_1 * 2;
        param_2 = param_2 * 2 + (uint)bVar3;
      } while ((param_2 & 0xfff00000) == 0);
    }
    else if ((param_2 & 0xffe00000) != 0) {
      do {
        unaff_EDI = unaff_EDI + 0x100000;
        uVar1 = param_2 & 1;
        param_2 = param_2 >> 1;
        uVar1 = (uint)(CONCAT14(uVar1 != 0,param_1) >> 1);
        uVar2 = (uint)(CONCAT14((param_1 & 1) != 0,uVar2) >> 1);
        param_1 = uVar1;
      } while ((param_2 & 0xffe00000) != 0);
      param_1 = uVar1 + CARRY4(uVar2,uVar2);
      param_2 = param_2 + CARRY4(uVar1,(uint)CARRY4(uVar2,uVar2));
    }
    param_2 = param_2 & 0xfffff | unaff_EDI;
  }
  return CONCAT44(param_2,param_1);
}

