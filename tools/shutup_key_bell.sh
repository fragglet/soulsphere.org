#!/bin/sh
# 
# Change the pitch/duration of the keyboard bell to something less annoying.
#

gconftool-2  -t int -s /desktop/gnome/peripherals/keyboard/bell_pitch 100
gconftool-2  -t int -s /desktop/gnome/peripherals/keyboard/bell_duration 20

