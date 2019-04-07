$env:PYTHONPATH = pwd
$py = where.exe python | select -first 1
$PYBASE =(get-item (get-item $py).Directoryname).parent.FullName
$PYLIB="$PYBASE/library"
$PYSHARE="$PYBASE/share"

rm $PYBASE/lib/site-packages/enum

pyi-makespec --add-binary "$PYLIB\bin\plaidml.dll;." --add-data "$PYSHARE;.\share" `
--add-data ".\tsumaki\model_branch\incubator\data;.\tsumaki\model_branch\incubator\data" `
--hidden-import pywt._extensions._cwt `
--hidden-import distutils `
--onefile `
--paths .\tsumaki\tsumaki_protobuf `
.\bin\tsumaki



# Append the following lines to tsumaki.spec if error message whining about distutils module appears
# import distutils
# if distutils.distutils_path.endswith('__init__.py'): distutils.distutils_path = os.path.dirname(distutils.distutils_path)
pyinstaller tsumaki.spec

.\dist\tsumaki self
