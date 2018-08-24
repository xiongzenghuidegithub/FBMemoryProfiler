#!/bin/sh

# 如果 IF_FIRST_INSTALL=true，会判断目录 AppleDeveloper 如果不存在才安装证书。
# IF_FIRST_INSTALL=true sh update_certificate.sh

path=$TMPDIR"AppleDeveloper"

if [ $IF_FIRST_INSTALL ] && [ -d "${path}" ]
then
    exit 0
fi

echo ""
echo "⌛️ 安装开发者证书"
echo "file in : ${path}"

if [ ! -d "${path}" ]; then
    echo "not" 
    git clone git@git.in.zhihu.com:Team-iOS-Platform/AppleDeveloper.git "${path}"
fi

echo ""
echo "update :"
cd "${path}"
git pull origin master
ls

echo ""
echo "install :"
ruby install.rb 8J52SRPW6X

echo "✅ 执行成功"


