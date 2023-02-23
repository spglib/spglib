# How to use spglib example files

This is the README file for spglib examples for C.

## Examples of using spglib in a cmake project

See the local [`CMakeLists.txt`](CMakeLists.txt) file for how to use
spglib in a cmake project, using either native cmake options or pkg-config.

## Examples of linking to spglib manually

To manually compile `example.c`:

```console
$ gcc example.c -I[include directory] -L[library directory] -lsymspg -lm
```

where the `[include directory]` and `[library directory]` can be obtained
via `pkg-config --cflags --libs spglib`.
