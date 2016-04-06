# Introduction #
<b>FTK交叉编译/运行的步骤 </b>

# Details #

<pre>假设FTK所在目录为 ~/work/ftk/</pre>
<pre>1. 将~/work/ftk/tools/cross-build目录中cross.mk env.sh makedist.sh几个文件拷贝到ftk的上级目录（即~/work/）下</pre>
<pre>2. 编辑env.sh 内的编译器名和编译器所在目录</pre>
<pre>3. source env.sh 此步将设置环境变量</pre>
<pre>4. make -f cross.mk</pre>
<pre>5. 编译开始将会自动下载相应的包并自动解开编译----请保持网络连接或者请在ftk同级目录下手工创建<br>
packages目录(即~/work/packages)并按cross.mk内指定的包下载相应的包到packages目录下</pre>
<pre>6. 编译完成后，在~/work/下运行./makedist.sh生成发布包ftk-dist.tar.gz。</pre>
<pre>7.把ftk-dist.tar.gz解压到目标板的根目录下，然后运行/opt/ftk-arm/bin/desktop。(arm换成实际的ARCH)</pre>

注：
<pre>
1.请在确保编译的用户可以读写/opt目录。<br>
2.如果有的软件包链接失效了，请手工下载到packages目录下，或者修改cross.mk<br>
</pre>