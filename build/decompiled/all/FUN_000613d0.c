
undefined4 __regparm3 FUN_000613d0(int param_1,undefined4 param_2,undefined4 param_3)

{
  if (((*(byte *)(param_1 + 0xa6) & 2) != 0) && (DAT_000cef84 != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 10;
    *(byte *)(param_1 + 0xa6) = *(byte *)(param_1 + 0xa6) & 0xfd;
  }
  if (((*(byte *)(param_1 + 0xa7) & 4) != 0) && (DAT_000cef84 != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 10;
    *(byte *)(param_1 + 0xa7) = *(byte *)(param_1 + 0xa7) & 0xfb;
  }
  if (((*(byte *)(param_1 + 0xa6) & 1) != 0) && (DAT_000cef88 != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 0xb;
    *(byte *)(param_1 + 0xa6) = *(byte *)(param_1 + 0xa6) & 0xfe;
  }
  if (((*(byte *)(param_1 + 0xa6) & 0x40) != 0) && (DAT_000cef88 != 0)) {
    *(undefined4 *)(param_1 + 0xee) = 0xb;
    *(byte *)(param_1 + 0xa6) = *(byte *)(param_1 + 0xa6) & 0xbf;
  }
  FUN_000504e0(param_2,param_3);
  return param_2;
}

