
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_00041320(float param_1)

{
  int iVar1;
  float *pfVar2;
  longdouble lVar3;
  float unaff_retaddr;
  float local_10;
  undefined1 local_c [12];
  
  if (0.0 <= param_1) {
    local_10 = 3.74269e-40;
    iVar1 = FUN_00076bcc();
    pfVar2 = (float *)local_c;
    lVar3 = ((longdouble)iVar1 / (longdouble)_DAT_00091671) * (longdouble)param_1;
  }
  else {
    local_10 = param_1 * _DAT_00091675;
    lVar3 = (longdouble)FUN_00041320();
    pfVar2 = &local_10;
    lVar3 = lVar3 + (longdouble)unaff_retaddr;
  }
  *(float *)((int)pfVar2 + 4) = (float)lVar3;
  return *(undefined4 *)((int)pfVar2 + 8);
}

