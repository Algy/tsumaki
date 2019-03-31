#!/bin/sh

cd tsumaki-protobuf && protoc $(find . -name '*.proto') --python_out ../tsumaki-core/tsumaki_protobuf --cpp_out ../tsumaki-obs/protobuf
