
undefined8 __regparm2
FUN_0001ded4(uint param_1,undefined4 param_2,int *param_3,int *param_4,uint param_5,uint param_6)

{
  undefined1 uVar1;
  uint uVar2;
  int iVar3;
  
  *param_3 = 0x3fff;
  *param_4 = 0x3fff;
  iVar3 = 0;
  out(0x201,(char)param_1);
  uVar2 = param_1;
LAB_0001def6:
  do {
    iVar3 = iVar3 + 1;
    if (iVar3 == 0x3fff) goto LAB_0001df2c;
    uVar1 = in(0x201);
    uVar2 = CONCAT31((int3)(uVar2 >> 8),uVar1);
    if ((*param_3 == 0x3fff) && ((param_5 & uVar2) == 0)) {
      *param_3 = iVar3;
    }
    if (*param_4 == 0x3fff) {
      if ((param_6 & uVar2) != 0) goto LAB_0001def6;
      *param_4 = iVar3;
    }
    if (*param_3 != 0x3fff) {
LAB_0001df2c:
      return CONCAT44(param_2,param_1);
    }
  } while( true );
}

