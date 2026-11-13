
int __regparm3 FUN_00080f58(int param_1)

{
  *(undefined ***)(param_1 + 0x28) = &PTR_FUN_000a6b98;
  if ((*(byte *)(param_1 + 0x24) & 2) != 0) {
    thunk_FUN_00075302();
  }
  return param_1;
}

