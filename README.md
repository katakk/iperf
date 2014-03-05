##iperf2.0.5 p27 那月ちゃんセーフティモード

* 速度測定用のツール。`iperf 2.0.5` のローカルコピー   

####おもな変更点

* `BSD``MIT`ライクライセンス`getoptとkernellistはopenbsd由来に変更` https://github.com/katakk/iperf/issues/14   
* パッチ当て済 `27`まで完了 https://github.com/katakk/iperf/issues/3   
* 不要ファイル削除 https://github.com/katakk/iperf/issues/1   
* strcpy などはstrncpyに変更 https://github.com/katakk/iperf/issues/7  
* Windows 用 GUIの追加 https://github.com/katakk/iperf/issues/8  
* WSA APIに変更  
* VC++2005でビルド通したきゅん `iperf.sln`  
* IPTOS周りWSAIoctl系に変更 https://github.com/katakk/iperf/issues/32  

![iperfgui](https://raw.github.com/katakk/iperf/master/iperfgui.png)

####使い方

* サーバ(送信側) `iperf -c 10.0.0.1 -w256k -i1 -t60` 
* クライアント(受信側) `iperf -s -w256k -i1`
* でデーターやりとりして速度をみるきゅん   

* コンパイルは `$ make  ` インストールは ` $ cp ./src/iperf  /usr/local/bin/iperf ` きゅん    
 



