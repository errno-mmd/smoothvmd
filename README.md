# smoothvmd

smoothvmd はモーションキャプチャーなどで生成されたVMDフォーマットのモーションデータを平滑化し、
キーフレームを間引きます。

## 必要なもの

予め下記のライブラリをインストールする必要があります。

- boost
- [ICU](http://site.icu-project.org/) - International Components for Unicode
- [Eigen](http://eigen.tuxfamily.org/)

また、ビルドには下記のツールが必要です。

- CMake
- GNU Make
- GNU C++ Compiler

## 動作確認済み環境

Ubuntu Linux 19.04

## ビルド方法

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

## 使い方

```
$ smoothvmd [options] input.vmd output.vmd
options:
  --help                help message
  --cutoff arg          cutoff frequency [Hz]
  --th_pos arg          threshold(position) for keyframe reduction
  --th_rot arg          threshold(rotation) for keyframe reduction [degree]
  --th_morph arg        threshold(morph) for keyframe reduction
  --fps_in arg          frame rate of input motion file
  --fps_out arg         frame rate of output motion file
  --bezier              bezier curve interpolation
```
上記コマンドを実行すると、smoothvmd は input.vmd を読み込んでモーションの平滑化と間引きを行い、
結果を output.vmd に出力します。

## ライセンス

MITライセンスです。詳しくは [LICENSE](LICENSE) ファイルを参照してください。

