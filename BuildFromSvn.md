# Introduction #

How to build ftk from svn source code?

# Details #

<b>download:</b>
<pre>
svn checkout http://ftk.googlecode.com/svn/trunk/ ftk-read-only<br>
</pre>
<b>configure:</b>
<pre>
cd ftk-read-only<br>
./autogen.sh<br>
./configure --with-backend=linux-x11:320x480<br>
</pre>
<b>build:</b>
<pre>
make && make install<br>
</pre>
<b>run:</b>
<pre>
desktop<br>
</pre>
更多编译选项和运行参数，请参考doc下的文档。