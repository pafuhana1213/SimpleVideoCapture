# SimpleVideoCapture
![demo](https://github.com/pafuhana1213/Screenshot/blob/master/SimpleVideoCaptureDemo.gif)

# はじめに（必ず読んでね❤）
昨今のゲームコンソールでは一般的な「一定時間前からのプレイ画面を動画にする」機能をWindowsで実現するためのプラグインです。録画の開始・終了はBP/C++から呼び出せるため、ユーザの操作だけでなくゴール時などの特定のタイミングで録画・保存を行うことも可能です。また、statの表示も録画することができるため自動テストなどのデバッグ用途でも便利かと思います。

本プラグインはUE4に標準で組み込まれているWindows専用機能を使用しているため **Windows以外の環境では動作しません** 。とあるプラットフォームでは動作する可能性がありますがサポート対象外としています。

とある不具合を回避するため、**デフォルトではStandalone、またはPackageでしか録画機能は動作しません。** PIE、New Editor Window でも動作させたい場合は後述の対応を行ってください。

動画ファイルは以下の場所に保存されます。
- Standlone  : ```<PROJECT_ROOT>/Saved/VideoCaptures```
- Package    : ```C:/Users/<USER_NAME>/AppData/Local/<PROJECT_NAME>/Saved/VideoCaptures```

## 最も重要 かつ 注意する必要があること
- デフォルトではこのプラグインを使用するUE4プロジェクトの配置場所に制限がかかります
- Shipping設定のPackageで正常に動作しません
- 4K解像度（ 3840x2160 ）以上の映像を録画しようとすると必ずクラッシュします

**ただし、これらの制限に関しても後述の対応で回避可能です。**

# 検証環境
- Windows10
- UE4.25.4 
  - 恐らく、UE4.24や26でも動くと思います。たぶん

# 使い方
## プロジェクトを指定の位置に配置（後述の対応でスキップ可能）
**エンジンと同じ階層に配置する必要があります。**　

例えば、ランチャー版のUE4.25の場合は```C:/Program Files/Epic Games/UE_4.25/ ``` 以下に配置することになります。

もしエンジンと同じ階層以外に配置すると、本プラグインを用いて動画を保存した際に以下のエラーが出力されます。
```
WMF: Error: `MFCreateSinkWriterFromURL(Filename, nullptr, nullptr, Writer.GetInitReference())` failed: 0x8007007B - ファイル名、ディレクトリ名、またはボリューム ラベルの構文が間違っています。
```

## プラグインを有効に
- プロジェクトのPlugins/SimpleVideoCaptureフォルダにSimpleVideoCaptureプラグイン一式を配置します
- エディタ上などでSimpleVideoCaptureプラグインを有効にします

## 録画開始
下図のように、SimpleVideoCaptureSubsystem から StartVideoCapture を使用します
- FileName : 保存する動画ファイルの名前
- FileSuffixType : 名前の後ろに付加する文字列の形式
- Recording Length Second : 保存する動画ファイルの長さ（録画時間ではありません）
![demo](https://github.com/pafuhana1213/Screenshot/blob/master/SimpleVideoCaptureDemo1.png)

## 録画終了
下図のように、SimpleVideoCaptureSubsystem から SaveVideoCapture を使用します。
なお、ノードにあるTitle / Commentピンは指定しても全く意味がありません。
（Windows環境の場合での話です。とあるプラットフォームでは何か意味があるかもしれません）
![demo](https://github.com/pafuhana1213/Screenshot/blob/master/SimpleVideoCaptureDemo2.png)

その他に関しては、サンプルプロジェクトのレベルBPにてご確認ください。

# 制限解除するための方法について
本プラグインはUE4標準機能をそのまま用いている関係で前述の制限・不具合が存在します。プロジェクト・プラグインからそれらを解決できれば良かったのですが、残念ながらエンジン外からは触る事ができない領域だったため、これから説明するエンジン改造によって解決してください。

## プロジェクトの配置場所・Shippingでの動作について
```Engine\Source\Runtime\Windows\WindowsPlatformFeatures\Private\HighlightRecorder.cpp``` の ```FHighlightRecorder::InitialiseMp4Writer``` 関数 における
```
FString FullFilename = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*(VideoCaptureDir + Filename)); 
```
を
```
FString FullFilename = Filename;
```
に変更すれば解決します。  
詳しい理由は割愛しますが、エンジン直下以外だと下図のようにFullFilenameに正常なファイルパスが入らないためです。
![demo](https://github.com/pafuhana1213/Screenshot/blob/master/SimpleVideoCaptureDemo3.png)

## 4K解像度（ 3840x2160 ）の映像を録画する方法について
本プラグインでは録画処理にNvVideoEncoderを使用しています。そして、4K解像度の動画を作成する場合はH.264の圧縮レベルを5.2に設定する必要があるのですが、デフォルトでは5.1になっているため不具合が発生します。
そして、H.264の圧縮レベルを5.2にするためには以下のような対応を入れる必要があります。
```
// Engine\Source\Runtime\GameplayMediaEncoder\Private\GameplayMediaEncoder.cpp
bool FGameplayMediaEncoder::Initialize()
{
...
	else
	{
		UE_LOG(GameplayMediaEncoder, Fatal, TEXT("GameplayMediaEncoder.ResY can only have a value of 720 or 1080"));
		return false;
	}
　
	// add for changing H.264 compression level to 5.2
	VideoConfig.Preset = AVEncoder::FVideoEncoderConfig::EPreset::LowLatency;

	// Specifying 0 will completely disable frame skipping (therefore encoding as many frames as possible)
	FParse::Value(FCommandLine::Get(), TEXT("GameplayMediaEncoder.FPS="), VideoConfig.Framerate);
  
 ...
```

ちなみに、PixelStreamingでもNvVideoEncoderを使用しているため、同じく4K解像度の映像を使用する際はH.264の圧縮レベルを5.2にする必要があります。  
詳しくは [Pixel Streaming リファレンス](https://docs.unrealengine.com/ja/SharingAndReleasing/PixelStreaming/PixelStreamingReference/index.html) をご確認ください。

##　PIE、NewEditorWindowでも動作させる方法について
上述のH.264の圧縮レベルを5.2にする対応を入れないとクラッシュしやすかったので、プラグイン側で制限をかけていました。

```
#if !USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR
	if (GetWorld()->WorldType != EWorldType::Game)
	{
		UE_LOG(SimpleVideoCaptureSubsystem, Warning, TEXT("SimpleVideoCaptureSubsystem can be used only in Standalone or Package if USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR is Disabled"));
		return false;
	}
#endif
```

なので、```Plugins/SimpleVideoCapture/Source/SimpleVideoCapture/Private/SimpleVideoCaptureSubsystem.cpp```の```#define USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR```を1にすればPIE、NewEditorWindowでも録画可能になります。

ただし動作は不安定です。ご注意ください。
