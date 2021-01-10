#!/bin/bash

# This file is part of the Home2L project.
#
# (C) 2015-2021 Gundolf Kiefer
#
# Home2L is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Home2L is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Home2L. If not, see <https://www.gnu.org/licenses/>.



# Setup ...
cd icons
DST_DIR=../icons.build
FLOORPLAN_SVG=../floorplan-template.svg    # floorplan template
rm -fr $DST_DIR
mkdir -p $DST_DIR



# SVG icons...
for f in ic-*.svg; do
  echo -n "ICON $f:"
  n=${f%%.svg}
  inkscape $n.svg -w 96 -h 96 -e $n.png >/dev/null 2>&1 || exit 3
    # FIXME [2019-12-22]: All stderr output is redirected to /dev/null here
    #   because otherwise inkscape produces a lot of dbus-related
    #   warnings. With a later version inkscape, the "2>&1" clause may and
    #   should be removed again.
    # NOTE: This refers to all invocations of 'inkscape' in this file!
  for s in 24 48 96; do
    #convert $n.png -geometry $sx$s $n-$s.bmp # -negate
    convert $n.png -geometry $sx$s -channel A -separate -colors 256 -compress None BMP3:$n-$s.bmp
    mv $n-$s.bmp $DST_DIR
    echo -n " $s"
  done
  rm $n.png
  echo
done



# Floorplan template icons...

#   Get available templates...
TPLS_RAW=`inkscape $FLOORPLAN_SVG -S 2>/dev/null | grep ^tpl\.`
TPLS=""
for T in $TPLS_RAW; do
  OBJ_NAME=${T%%,*}
  BASE=${OBJ_NAME#tpl.}
  TPLS="$TPLS $BASE"
done
#~ echo "### Templates: $TPLS"

#   Convert them...
for BASE in $TPLS; do
  PNG=$DST_DIR/$BASE.png    # base .png from inkscape
  BMP=$DST_DIR/fp-$BASE.bmp # destination file name
  echo "FLOORPLAN $BASE"
  inkscape $FLOORPLAN_SVG -i tpl.$BASE -d 768 -e $PNG >/dev/null 2>&1     # '-d 768' refers to a scale factor of 8 (id 3) (96 * 8 dpi)
  convert $PNG -channel R -separate -colors 256 -compress None BMP3:$BMP
  rm $PNG
done



# Droid digits...
for f in droids-empty droids-digits; do
  echo "IMAGE $f"
  convert freedroid/$f.png -channel A -separate -colors 256 -compress None BMP3:$DST_DIR/$f.bmp
done



# Others...
for n in phone-incall phone-ringing phone-ringing-door phone-ringing-gate pic-wakeup; do
  if [ -e $n.svg ]; then
    echo "IMAGE $n"
    inkscape $n.svg -w 480 -h 480 -e $n.png > /dev/null 2>&1
    convert $n.png -geometry 480x480 -channel A -separate -colors 256 -compress None BMP3:$n.bmp
    mv $n.bmp $DST_DIR
    rm $n.png
  fi
  if [ -e $n.png ]; then
    echo "IMAGE $n"
    convert $n.png tmp.bmp
    convert tmp.bmp -channel R -separate -colors 256 -compress None BMP3:$DST_DIR/$n.bmp
    rm tmp.bmp
  fi
done



# Fix permissions...
chmod 755 $DST_DIR
chmod 644 $DST_DIR/*.bmp
