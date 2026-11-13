
longlong __regparm3 FUN_00026860(undefined4 param_1,uint param_2)

{
  int iVar1;
  undefined4 *puVar2;
  
  iVar1 = FUN_00076a69();
  if (iVar1 == 0) {
    iVar1 = FUN_0005c320();
    if (iVar1 != 0) {
      iVar1 = FUN_0006cd40();
      *(undefined4 *)(iVar1 + 0x124) = 0;
      *(undefined4 *)(iVar1 + 0x128) = 0;
      *(undefined4 *)(iVar1 + 300) = 0;
      *(undefined ***)(iVar1 + 0x130) = &PTR_LAB_00098278;
      *(undefined ***)(iVar1 + 8) = &PTR_LAB_00099258;
      *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_00099250;
      return CONCAT44(param_2,iVar1);
    }
  }
  else {
    iVar1 = FUN_00076a69();
    if (iVar1 == 0) {
      iVar1 = FUN_0005c320();
      if (iVar1 != 0) {
        iVar1 = FUN_0003d8c0();
        *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009916c;
        *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_00099164;
        return CONCAT44(param_2,iVar1);
      }
    }
    else {
      iVar1 = FUN_00076a69();
      if ((iVar1 == 0) && (iVar1 = FUN_0005c320(), iVar1 != 0)) {
        FUN_00053ff0();
        puVar2 = (undefined4 *)FUN_00044fe0();
        puVar2[3] = 0;
        puVar2[4] = 0;
        puVar2[5] = &PTR_LAB_00098278;
        puVar2[6] = 0;
        puVar2[7] = 0;
        puVar2[8] = &PTR_LAB_00098278;
        puVar2[0xe] = 0;
        puVar2[0xf] = &PTR_LAB_00098278;
        puVar2[0xd] = 0;
        *(undefined ***)((int)puVar2 + -0xe6) = &PTR_LAB_00099080;
        *puVar2 = &PTR_LAB_0009905c;
        *(undefined ***)((int)puVar2 + -0xe2) = &PTR_LAB_00099054;
        return CONCAT44(param_2,(int)puVar2 + -0xee);
      }
    }
  }
  return (ulonglong)param_2 << 0x20;
}

