#!/bin/sh
PYBASE="$(dirname $(dirname $(which python)))"
PYLIB="$PYBASE/lib"
PYSHARE="$PYBASE/share"
PYINCLUDE="$PYBASE/include"


#--add-data "$PYLIB:./lib" \
#--add-data "$PYINCLUDE:./include" \

pyinstaller \
--add-binary "$PYLIB/libplaidml.so:." \
--add-data "$PYSHARE:./share" \
--add-data "./tsumaki/model_branch/incubator/data:./tsumaki/model_branch/incubator/data" \
--hidden-import 'pywt._extensions._cwt' \
--onefile \
--paths ./tsumaki/tsumaki_protobuf \
./bin/tsumaki

./dist/tsumaki self-check