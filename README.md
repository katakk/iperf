##iperf2.0.5 p27 那月ちゃんセーフティモード

* `BSD``MIT`ライクライセンス
* 速度測定用のツール。`iperf 2.0.5` のローカルコピー   
* Windows 用 GUIの追加
* VC++2005でビルド通したきゅん `iperf.sln`
* パッチ当て済 `27`まで完了 https://github.com/katakk/iperf/issues/3  
* コンパイルは `$ make  ` インストールは ` $ cp ./src/iperf  /usr/local/bin/iperf ` きゅん    
* 不要ファイル削除

![iperfgui](https://raw.github.com/katakk/iperf/master/iperfgui.png)

####使い方

* サーバ(送信側) `iperf -c 10.0.0.1 -w256k -i1 -t60` 
* クライアント(受信側) `iperf -s -w256k -i1`
* でデーターやりとりして速度をみるきゅん   

 



