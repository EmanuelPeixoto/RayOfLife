#!/usr/bin/env bash

premake5 gmake2
make config=release
exec ./bin/Release/Grid-Test
