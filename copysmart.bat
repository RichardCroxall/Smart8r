echo Copying smart code
echo on
del g:\smart8r
mkdir g:\smart8r
mkdir g:\smart8r\include
mkdir g:\smart8r\huehelper
mkdir g:\smart8r\huehelper\include
mkdir g:\smart8r\hueplusplus
mkdir g:\smart8r\hueplusplus\src
mkdir g:\smart8r\hueplusplus\include
mkdir g:\smart8r\hueplusplus\include\json

copy *.cpp g:\smart8r\
copy huehelper\*cpp g:\smart8r\huehelper\
copy include\*h g:\smart8r\include\
copy huehelper\include\*h g:\smart8r\huehelper\include\
copy hueplusplus\src\*cpp g:\smart8r\hueplusplus\src
copy hueplusplus\include\*h g:\smart8r\hueplusplus\include\
copy hueplusplus\include\json\*.* g:\smart8r\hueplusplus\include\json\
copy makefile g:\smart8r\
copy mounta g:\smart8r\
copy mountb g:\smart8r\
copy umount g:\smart8r\
copy smart.smt g:\smart8r\
copy hub.json g:\smart8r\
copy start g:\smart8r\

echo finished copying code.
