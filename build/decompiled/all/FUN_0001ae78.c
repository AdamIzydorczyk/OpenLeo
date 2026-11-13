
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0001ae78(void)

{
  int iVar1;
  
  iVar1 = DAT_0003de24 * DAT_0003de3c;
  DAT_000a893c = DAT_000341bc + _DAT_000341cc;
  DAT_000a8940 = DAT_000a893c + iVar1;
  DAT_000a8944 = DAT_000a8940 + iVar1;
  DAT_000a8948 = DAT_000a8944 + iVar1;
  _DAT_000a894c = DAT_000c41c8 >> 2;
  do {
    DAT_000a8970 = 1;
    FUN_0001acbb();
    _DAT_000a894c = _DAT_000a894c - DAT_000a8970;
  } while (_DAT_000a894c != 0);
  return;
}

