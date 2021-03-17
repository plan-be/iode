@SET BDS=C:\Program Files (x86)\Embarcadero\Studio\17.0
@SET BDSCOMMONDIR=C:\Users\Public\Documents\Embarcadero\Studio\17.0
@SET FrameworkDir=C:\Windows\Microsoft.NET\Framework\
@SET FrameworkVersion=v4.0.30319
@SET FrameworkSDKDir=
@SET PATH=%FrameworkDir%%FrameworkVersion%;%FrameworkSDKDir%;%PATH%

del *.exe
MSBuild
copy IodeComServer.exe iodecom.exe
DigiCertUtil.exe sign /noinput /sha1 "c8c28b4300e274f9cf437244f913cc99bfef109b" "iodecom.exe"
