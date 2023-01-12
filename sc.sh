#!/bin/bash

# 获取文件夹中所有 u 开头的文件
files=$(find . -type f -name "u*")

# 对每个文件进行重命名
for file in $files; do
  # 获取文件的路径和名称
  path=$(dirname $file)
  name=$(basename $file)
  # 去掉文件名的第一个字符
  new_name=${name:1}
  # 重命名文件
  mv $file $path/$new_name
done
