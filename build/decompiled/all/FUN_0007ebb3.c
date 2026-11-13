
void __regparm3 FUN_0007ebb3(uint param_1,uint param_2,uint param_3)

{
  uint uVar1;
  uint unaff_EBX;
  uint unaff_ESI;
  int iVar2;
  int iVar3;
  uint unaff_EDI;
  bool bVar4;
  bool bVar5;
  
  iVar2 = (unaff_ESI & 0x7fff) - (unaff_EDI & 0x7fff);
  if ((unaff_ESI & 0x7fff) < (unaff_EDI & 0x7fff)) {
    return;
  }
LAB_0007ebc7:
  bVar4 = param_3 < param_2;
  if (((param_3 != param_2) || (bVar4 = unaff_EBX < param_1, unaff_EBX != param_1)) &&
     (iVar3 = iVar2, !bVar4)) goto LAB_0007ebda;
  do {
    bVar4 = param_1 < unaff_EBX;
    param_1 = param_1 - unaff_EBX;
    param_2 = (param_2 - param_3) - (uint)bVar4;
    iVar3 = iVar2;
LAB_0007ebda:
    while( true ) {
      iVar2 = iVar3 + -1;
      if (iVar3 < 1) {
        if ((param_1 == 0) && (param_2 == 0)) {
          return;
        }
        for (; -1 < (int)param_2; param_2 = param_2 * 2 + (uint)bVar4) {
          bVar4 = CARRY4(param_1,param_1);
          param_1 = param_1 * 2;
        }
        return;
      }
      bVar4 = CARRY4(param_1,param_1);
      param_1 = param_1 * 2;
      bVar5 = CARRY4(param_2,param_2);
      uVar1 = param_2 * 2;
      param_2 = uVar1 + bVar4;
      if (bVar5 || CARRY4(uVar1,(uint)bVar4)) break;
      iVar3 = iVar2;
      if ((int)param_2 < 0) goto LAB_0007ebc7;
    }
  } while( true );
}

