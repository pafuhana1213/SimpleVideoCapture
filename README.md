# SimpleVideoCapture
![demo](https://github.com/pafuhana1213/Screenshot/blob/master/SimpleVideoCaptureDemo.gif)

# はじめに（必ず読んでね❤）
昨今のゲームコンソールでは一般的な「一定時間前からのプレイ画面を動画にする」機能をWindowsで実現するためのプラグインです。録画の開始・終了はBP/C++から呼び出せるため、ユーザの操作だけでなくゴール時などの特定のタイミングで録画・保存を行うことも可能です。また、statの表示も録画することができるため自動テストなどのデバッグ用途でも便利かと思います。

本プラグインはUE4に標準で組み込まれているWindows専用機能を使用しているため **Windows以外の環境では動作しません** 。とあるプラットフォームでは動作する可能性がありますがサポート対象外としています。

後述の不具合を回避するため、**デフォルトではStandalone、またはPackageでしか録画機能は動作しません。** PIE、New Editor Window でも動作させたい場合は後述の対応を行ってください。

動画ファイルは以下の場所に保存されます。
- Standlone : <PROJECT_ROOT>/Saved/VideoCaptures
- Package : C:/Users/<USER_NAME>/AppData/Local/<PROJECT_NAME>/Saved/VideoCaptures



# 使い方
## プラグインを有効に
- プロジェクトのPlugins/SimpleVideoCaptureフォルダにSimpleVideoCaptureプラグイン一式を配置します
- エディタ上などでSimpleVideoCaptureプラグインを有効にします

## 録画開始
下図のように、SimpleVideoCaptureSubsystem から StartVideoCapture を使用します
FileNameは保存する動画ファイルの名前、FileSuffixTypeは名前の後ろに付加する文字列の形式を指定できます
![demo](https://github.com/pafuhana1213/Screenshot/blob/master/SimpleVideoCaptureDemo1.png)

