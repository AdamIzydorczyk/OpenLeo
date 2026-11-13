
void __regparm3 FUN_0007fb8a(uint param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  undefined4 *unaff_EBX;
  undefined4 unaff_ESI;
  undefined4 uVar2;
  double dVar3;
  
  if ((param_1 & 0x40) == 0) {
    if ((param_1 & 0x80) == 0) {
      if ((param_1 & 0x100) == 0) {
        if ((param_1 & 0x800) == 0) {
          if ((param_1 & 0x200) == 0) {
            if ((param_1 & 0x400) != 0) {
              unaff_ESI = 5;
            }
          }
          else {
            unaff_ESI = 6;
          }
        }
        else {
          unaff_ESI = 4;
        }
      }
      else {
        unaff_ESI = 3;
      }
    }
    else {
      unaff_ESI = 2;
    }
  }
  else {
    unaff_ESI = 1;
  }
  if ((param_1 & 0x1000) == 0) {
    if ((param_1 & 0x2000) == 0) {
      if ((param_1 & 0x4000) == 0) {
        if ((param_1 & 0x8000) == 0) {
          uVar2 = *unaff_EBX;
          uVar1 = unaff_EBX[1];
        }
        else {
          uVar1 = DAT_000a6bc8;
          uVar2 = DAT_000a6bc4;
        }
        dVar3 = (double)CONCAT44(uVar1,uVar2);
      }
      else {
        dVar3 = 1.0;
      }
    }
    else {
      dVar3 = 0.0;
    }
  }
  else {
    dVar3 = -(double)CONCAT44(DAT_000a6bc8,DAT_000a6bc4);
  }
  FUN_00081054(unaff_ESI,(&PTR_DAT_000ad558)[param_1 & 0x1f],*param_2,param_2[1],*unaff_EBX,
               unaff_EBX[1],dVar3);
  return;
}

