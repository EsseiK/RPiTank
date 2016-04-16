# RPiTank
Progress Technologies : Raspberry Pi 勉強会 (2015) の開発物です。

[build command]
make clean
make

実行にはsudoを付けてください（wiringPiを使用するため）
sudo ./RPiTank

カメラストリーミングをする際には相手のIPアドレスを変更してビルドし直してください。
相手のIPをスレッド間で共有できれば解決。

競合すると<<<<とか出る
手動で修正