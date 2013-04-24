#! /bin/sh
$EXTRACTRC `find src -name '*.rc'` >> rc.cpp
$EXTRACTRC `find src/ui -name *.ui` >> rc.cpp
$EXTRACTRC `find src -name '*.kcfg'` >> rc.cpp
$EXTRACTRC --context="Language Title" --tag-group=none --tag=i18nTitle `find data/languages/ -name '*.xml'` >> rc.cpp
$XGETTEXT rc.cpp `find src/ -name \*.qml -or -name \*.cpp` -o $podir/artikulate.pot
