#! /bin/sh
$EXTRACTRC `find src -name '*.rc'` >> rc.cpp
$EXTRACTRC `find src/ui -name *.ui` >> rc.cpp
$EXTRACTRC `find src -name '*.kcfg'` >> rc.cpp
$XGETTEXT rc.cpp `find src/ -name \*.qml -or -name \*.cpp` -o $podir/artikulate.pot

