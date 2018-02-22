# moboot-pre3 - Mobile Open Bootloader for the HP Pre 3

This is a fork of [moboot](https://github.com/jcsullins/moboot), the Mobile Open Bootloader originally made for the HP Touchpad, which [xndcn](https://github.com/xndcn/moboot-for-veer) made available for the HP Veer too.

## Building `moboot`

Building `moboot` for the Pre 3 is easy.

Download [this exact GCC distribution](https://sourcery.mentor.com/public/gnu_toolchain/arm-none-linux-gnueabi/arm-2009q1-203-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2) and place it somewhere in your filesystem.

Add the `bin` subdirectory of the extracted archive file to your `$PATH`, and then call `make`.

You'll find `moboot`'s uImage ready to be membooted in `build-pre3`, it's called `lk.uImage` - memboot it through novacom:

```
$ novacom boot mem:// < lk.uImage
```

## Things you should know

 - for some strange reason, webOS likes to "disable" GPIO pins in a way that makes some of them unable to work even in moboot - make sure to boot into webOS and set your volume slider to "no sound" before using moboot, otherwise the menu will not work.
