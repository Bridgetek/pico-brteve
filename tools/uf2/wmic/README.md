# wmic support for the UF2 Converter

## Introduction
This folder store a copy of wmic executable.

The [uf2conv4eve.py](#) need to get a list of hard drive on Window when operating.

If it couldn't find any drive, it would try again with wmic command.

On Window 11, wmic command is no longer available by default, so the [uf2conv4eve.py](#) will call wmic\wmix.exe.