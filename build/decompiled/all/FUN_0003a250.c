
longlong __regparm3 FUN_0003a250(undefined4 param_1,uint param_2)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  
  iVar1 = FUN_00076a69();
  if (iVar1 == 0) {
    iVar1 = FUN_0005c320();
    if (iVar1 != 0) {
      iVar1 = FUN_00052860();
      *(undefined4 *)(iVar1 + 0x3c) = 0;
      *(undefined4 *)(iVar1 + 0x40) = 0;
      *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009e294;
      *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009e28c;
      puVar2 = (undefined4 *)FUN_00044fe0();
      puVar2[5] = 0;
      puVar2[6] = 0;
      puVar2[7] = 0;
      puVar2[8] = &PTR_FUN_00098274;
      puVar2[4] = 0;
      puVar2[-0xf] = &PTR_LAB_0009f7b4;
      *puVar2 = &PTR_LAB_0009f790;
      puVar2[-0xe] = &PTR_LAB_0009f788;
      return CONCAT44(param_2,puVar2 + -0x11);
    }
  }
  else {
    iVar1 = FUN_00076a69();
    if (iVar1 == 0) {
      iVar1 = FUN_0005c320();
      if (iVar1 != 0) {
        iVar1 = FUN_00053ff0();
        *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009f854;
        *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009f84c;
        return CONCAT44(param_2,iVar1);
      }
    }
    else {
      iVar1 = FUN_00076a69();
      if (iVar1 == 0) {
        iVar1 = FUN_0005c320();
        if (iVar1 != 0) {
          iVar1 = FUN_00053ff0();
          *(undefined4 *)(iVar1 + 0xf2) = 0;
          *(undefined4 *)(iVar1 + 0xf6) = 0;
          *(undefined4 *)(iVar1 + 0xfa) = 0;
          *(undefined ***)(iVar1 + 0xfe) = &PTR_FUN_00098274;
          *(undefined4 *)(iVar1 + 0xee) = 0;
          *(undefined ***)(iVar1 + 8) = &PTR_LAB_0009f934;
          *(undefined ***)(iVar1 + 0xc) = &PTR_LAB_0009f92c;
          return CONCAT44(param_2,iVar1);
        }
      }
      else {
        iVar1 = FUN_00076a69();
        if (iVar1 == 0) {
          iVar1 = FUN_0005c320();
          if (iVar1 != 0) {
            FUN_00053ff0();
            puVar2 = (undefined4 *)FUN_00044fe0();
            puVar2[3] = 0;
            puVar2[4] = 0;
            puVar2[5] = &PTR_LAB_00098278;
            puVar2[7] = 0;
            puVar2[8] = &PTR_LAB_00098278;
            puVar2[6] = 0;
            *(undefined ***)((int)puVar2 + -0xe6) = &PTR_LAB_0009fa38;
            *puVar2 = &PTR_LAB_0009fa14;
            *(undefined ***)((int)puVar2 + -0xe2) = &PTR_LAB_0009fa0c;
            return CONCAT44(param_2,(int)puVar2 + -0xee);
          }
        }
        else {
          iVar1 = FUN_00076a69();
          if ((iVar1 == 0) && (iVar1 = FUN_0005c320(), iVar1 != 0)) {
            uVar3 = FUN_0003a110();
            return CONCAT44(param_2,uVar3);
          }
        }
      }
    }
  }
  return (ulonglong)param_2 << 0x20;
}

