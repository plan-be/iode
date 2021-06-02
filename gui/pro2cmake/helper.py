#############################################################################
##
## Copyright (C) 2018 The Qt Company Ltd.
## Contact: https://www.qt.io/licensing/
##
## This file is part of the plugins of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:GPL-EXCEPT$
## Commercial License Usage
## Licensees holding valid commercial Qt licenses may use this file in
## accordance with the commercial license agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and The Qt Company. For licensing terms
## and conditions see https://www.qt.io/terms-conditions. For further
## information use the contact form at https://www.qt.io/contact-us.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU
## General Public License version 3 as published by the Free Software
## Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
## included in the packaging of this file. Please review the following
## information to ensure the GNU General Public License requirements will
## be met: https://www.gnu.org/licenses/gpl-3.0.html.
##
## $QT_END_LICENSE$
##
#############################################################################

import re
import typing


class LibraryMapping:
    def __init__(
        self,
        soName: str,
        packageName: typing.Optional[str],
        targetName: typing.Optional[str],
        *,
        resultVariable: typing.Optional[str] = None,
        extra: typing.List[str] = [],
        appendFoundSuffix: bool = True,
        emit_if: str = "",
    ) -> None:
        self.soName = soName
        self.packageName = packageName
        self.resultVariable = resultVariable
        self.appendFoundSuffix = appendFoundSuffix
        self.extra = extra
        self.targetName = targetName

        # if emit_if is non-empty, the generated find_package call
        # for a library will be surrounded by this condition.
        self.emit_if = emit_if

    def is_qt(self) -> bool:
        return self.packageName == "Qt" or self.packageName == "Qt5" or self.packageName == "Qt6"


_qt_library_map = [
    # Qt:
    LibraryMapping(
        "accessibility_support",
        "Qt6",
        "Qt::AccessibilitySupport",
        extra=["COMPONENTS", "AccessibilitySupport"],
    ),
    LibraryMapping(
        "androidextras", "Qt6", "Qt::AndroidExtras", extra=["COMPONENTS", "AndroidExtras"]
    ),
    LibraryMapping("animation", "Qt6", "Qt::3DAnimation", extra=["COMPONENTS", "3DAnimation"]),
    LibraryMapping(
        "application-lib", "Qt6", "Qt::AppManApplication", extra=["COMPONENTS", "AppManApplication"]
    ),
    LibraryMapping("bluetooth", "Qt6", "Qt::Bluetooth", extra=["COMPONENTS", "Bluetooth"]),
    LibraryMapping("bootstrap", "Qt6", "Qt::Bootstrap", extra=["COMPONENTS", "Bootstrap"]),
    # bootstrap-dbus: Not needed in Qt6!
    LibraryMapping("client", "Qt6", "Qt::WaylandClient", extra=["COMPONENTS", "WaylandClient"]),
    LibraryMapping(
        "clipboard_support", "Qt6", "Qt::ClipboardSupport", extra=["COMPONENTS", "ClipboardSupport"]
    ),
    LibraryMapping("coap", "Qt6", "Qt::Coap", extra=["COMPONENTS", "Coap"]),
    LibraryMapping("common-lib", "Qt6", "Qt::AppManCommon", extra=["COMPONENTS", "AppManCommon"]),
    LibraryMapping(
        "compositor", "Qt6", "Qt::WaylandCompositor", extra=["COMPONENTS", "WaylandCompositor"]
    ),
    LibraryMapping("concurrent", "Qt6", "Qt::Concurrent", extra=["COMPONENTS", "Concurrent"]),
    LibraryMapping("container", "Qt6", "Qt::AxContainer", extra=["COMPONENTS", "AxContainer"]),
    LibraryMapping("control", "Qt6", "Qt::AxServer", extra=["COMPONENTS", "AxServer"]),
    LibraryMapping(
        "core_headers", "Qt6", "Qt::WebEngineCore", extra=["COMPONENTS", "WebEngineCore"]
    ),
    LibraryMapping("core", "Qt6", "Qt::Core", extra=["COMPONENTS", "Core"]),
    LibraryMapping("coretest", "Qt6", "Qt::3DCoreTest", extra=["COMPONENTS", "3DCoreTest"]),
    LibraryMapping("crypto-lib", "Qt6", "Qt::AppManCrypto", extra=["COMPONENTS", "AppManCrypto"]),
    LibraryMapping("dbus", "Qt6", "Qt::DBus", extra=["COMPONENTS", "DBus"]),
    LibraryMapping("designer", "Qt6", "Qt::Designer", extra=["COMPONENTS", "Designer"]),
    LibraryMapping(
        "designercomponents",
        "Qt6",
        "Qt::DesignerComponents",
        extra=["COMPONENTS", "DesignerComponents"],
    ),
    LibraryMapping(
        "devicediscovery",
        "Qt6",
        "Qt::DeviceDiscoverySupport",
        extra=["COMPONENTS", "DeviceDiscoverySupport"],
    ),
    LibraryMapping(
        "devicediscovery_support",
        "Qt6",
        "Qt::DeviceDiscoverySupport",
        extra=["COMPONENTS", "DeviceDiscoverySupport"],
    ),
    LibraryMapping("edid", "Qt6", "Qt::EdidSupport", extra=["COMPONENTS", "EdidSupport"]),
    LibraryMapping("edid_support", "Qt6", "Qt::EdidSupport", extra=["COMPONENTS", "EdidSupport"]),
    LibraryMapping("eglconvenience", "Qt6", "Qt::EglSupport", extra=["COMPONENTS", "EglSupport"]),
    LibraryMapping(
        "eglfsdeviceintegration",
        "Qt6",
        "Qt::EglFSDeviceIntegration",
        extra=["COMPONENTS", "EglFSDeviceIntegration"],
    ),
    LibraryMapping(
        "eglfs_kms_support", "Qt6", "Qt::EglFsKmsSupport", extra=["COMPONENTS", "EglFsKmsSupport"]
    ),
    LibraryMapping("egl_support", "Qt6", "Qt::EglSupport", extra=["COMPONENTS", "EglSupport"]),
    # enginio: Not needed in Qt6!
    LibraryMapping(
        "eventdispatchers",
        "Qt6",
        "Qt::EventDispatcherSupport",
        extra=["COMPONENTS", "EventDispatcherSupport"],
    ),
    LibraryMapping(
        "eventdispatcher_support",
        "Qt6",
        "Qt::EventDispatcherSupport",
        extra=["COMPONENTS", "EventDispatcherSupport"],
    ),
    LibraryMapping("extras", "Qt6", "Qt::3DExtras", extra=["COMPONENTS", "3DExtras"]),
    LibraryMapping("fbconvenience", "Qt6", "Qt::FbSupport", extra=["COMPONENTS", "FbSupport"]),
    LibraryMapping("fb_support", "Qt6", "Qt::FbSupport", extra=["COMPONENTS", "FbSupport"]),
    LibraryMapping(
        "fontdatabase_support",
        "Qt6",
        "Qt::FontDatabaseSupport",
        extra=["COMPONENTS", "FontDatabaseSupport"],
    ),
    LibraryMapping("gamepad", "Qt6", "Qt::Gamepad", extra=["COMPONENTS", "Gamepad"]),
    LibraryMapping(
        "global", "Qt6", "Qt::Core", extra=["COMPONENTS", "Core"]
    ),  # manually added special case
    LibraryMapping("glx_support", "Qt6", "Qt::GlxSupport", extra=["COMPONENTS", "GlxSupport"]),
    LibraryMapping(
        "graphics_support", "Qt6", "Qt::GraphicsSupport", extra=["COMPONENTS", "GraphicsSupport"]
    ),
    LibraryMapping(
        "gsttools", "Qt6", "Qt::MultimediaGstTools", extra=["COMPONENTS", "MultimediaGstTools"]
    ),
    LibraryMapping("gui", "Qt6", "Qt::Gui", extra=["COMPONENTS", "Gui"]),
    LibraryMapping("help", "Qt6", "Qt::Help", extra=["COMPONENTS", "Help"]),
    LibraryMapping(
        "hunspellinputmethod",
        "Qt6",
        "Qt::HunspellInputMethod",
        extra=["COMPONENTS", "HunspellInputMethod"],
    ),
    LibraryMapping("input", "Qt6", "Qt::InputSupport", extra=["COMPONENTS", "InputSupport"]),
    LibraryMapping(
        "input_support", "Qt6", "Qt::InputSupport", extra=["COMPONENTS", "InputSupport"]
    ),
    LibraryMapping(
        "installer-lib", "Qt6", "Qt::AppManInstaller", extra=["COMPONENTS", "AppManInstaller"]
    ),
    LibraryMapping("knx", "Qt6", "Qt::Knx", extra=["COMPONENTS", "Knx"]),
    LibraryMapping("kmsconvenience", "Qt6", "Qt::KmsSupport", extra=["COMPONENTS", "KmsSupport"]),
    LibraryMapping("kms_support", "Qt6", "Qt::KmsSupport", extra=["COMPONENTS", "KmsSupport"]),
    LibraryMapping(
        "launcher-lib", "Qt6", "Qt::AppManLauncher", extra=["COMPONENTS", "AppManLauncher"]
    ),
    LibraryMapping("lib", "Qt6", "Qt::Designer", extra=["COMPONENTS", "Designer"]),
    LibraryMapping(
        "linuxaccessibility_support",
        "Qt6",
        "Qt::LinuxAccessibilitySupport",
        extra=["COMPONENTS", "LinuxAccessibilitySupport"],
    ),
    LibraryMapping("location", "Qt6", "Qt::Location", extra=["COMPONENTS", "Location"]),
    LibraryMapping("logic", "Qt6", "Qt::3DLogic", extra=["COMPONENTS", "3DLogic"]),
    LibraryMapping("macextras", "Qt6", "Qt::MacExtras", extra=["COMPONENTS", "MacExtras"]),
    LibraryMapping("main-lib", "Qt6", "Qt::AppManMain", extra=["COMPONENTS", "AppManMain"]),
    LibraryMapping(
        "manager-lib", "Qt6", "Qt::AppManManager", extra=["COMPONENTS", "AppManManager"]
    ),
    LibraryMapping(
        "monitor-lib", "Qt6", "Qt::AppManMonitor", extra=["COMPONENTS", "AppManMonitor"]
    ),
    LibraryMapping("mqtt", "Qt6", "Qt::Mqtt", extra=["COMPONENTS", "Mqtt"]),
    LibraryMapping("multimedia", "Qt6", "Qt::Multimedia", extra=["COMPONENTS", "Multimedia"]),
    LibraryMapping(
        "multimediawidgets",
        "Qt6",
        "Qt::MultimediaWidgets",
        extra=["COMPONENTS", "MultimediaWidgets"],
    ),
    LibraryMapping("network", "Qt6", "Qt::Network", extra=["COMPONENTS", "Network"]),
    LibraryMapping("networkauth", "Qt6", "Qt::NetworkAuth", extra=["COMPONENTS", "NetworkAuth"]),
    LibraryMapping("nfc", "Qt6", "Qt::Nfc", extra=["COMPONENTS", "Nfc"]),
    LibraryMapping("oauth", "Qt6", "Qt::NetworkAuth", extra=["COMPONENTS", "NetworkAuth"]),
    LibraryMapping(
        "openglextensions", "Qt6", "Qt::OpenGLExtensions", extra=["COMPONENTS", "OpenGLExtensions"]
    ),
    LibraryMapping("opengl", "Qt6", "Qt::OpenGL", extra=["COMPONENTS", "OpenGL"]),
    LibraryMapping(
        "package-lib", "Qt6", "Qt::AppManPackage", extra=["COMPONENTS", "AppManPackage"]
    ),
    LibraryMapping(
        "packetprotocol", "Qt6", "Qt::PacketProtocol", extra=["COMPONENTS", "PacketProtocol"]
    ),
    LibraryMapping(
        "particles", "Qt6", "Qt::QuickParticles", extra=["COMPONENTS", "QuickParticles"]
    ),
    LibraryMapping(
        "platformcompositor",
        "Qt6",
        "Qt::PlatformCompositorSupport",
        extra=["COMPONENTS", "PlatformCompositorSupport"],
    ),
    LibraryMapping(
        "platformcompositor_support",
        "Qt6",
        "Qt::PlatformCompositorSupport",
        extra=["COMPONENTS", "PlatformCompositorSupport"],
    ),
    LibraryMapping(
        "plugin-interfaces",
        "Qt6",
        "Qt::AppManPluginInterfaces",
        extra=["COMPONENTS", "AppManPluginInterfaces"],
    ),
    LibraryMapping("positioning", "Qt6", "Qt::Positioning", extra=["COMPONENTS", "Positioning"]),
    LibraryMapping(
        "positioningquick", "Qt6", "Qt::PositioningQuick", extra=["COMPONENTS", "PositioningQuick"]
    ),
    LibraryMapping("printsupport", "Qt6", "Qt::PrintSupport", extra=["COMPONENTS", "PrintSupport"]),
    LibraryMapping("purchasing", "Qt6", "Qt::Purchasing", extra=["COMPONENTS", "Purchasing"]),
    LibraryMapping("qmldebug", "Qt6", "Qt::QmlDebug", extra=["COMPONENTS", "QmlDebug"]),
    LibraryMapping("qmldevtools", "Qt6", "Qt::QmlDevTools", extra=["COMPONENTS", "QmlDevTools"]),
    LibraryMapping("qml", "Qt6", "Qt::Qml", extra=["COMPONENTS", "Qml"]),
    LibraryMapping("qmlmodels", "Qt6", "Qt::QmlModels", extra=["COMPONENTS", "QmlModels"]),
    LibraryMapping("qmltest", "Qt6", "Qt::QuickTest", extra=["COMPONENTS", "QuickTest"]),
    LibraryMapping(
        "qtmultimediaquicktools",
        "Qt6",
        "Qt::MultimediaQuick",
        extra=["COMPONENTS", "MultimediaQuick"],
    ),
    LibraryMapping(
        "quick3dassetimport",
        "Qt6",
        "Qt::Quick3DAssetImport",
        extra=["COMPONENTS", "Quick3DAssetImport"],
    ),
    LibraryMapping("quick3d", "Qt6", "Qt::Quick3D", extra=["COMPONENTS", "Quick3D"]),
    LibraryMapping(
        "quick3drender", "Qt6", "Qt::Quick3DRender", extra=["COMPONENTS", "Quick3DRender"]
    ),
    LibraryMapping(
        "quick3druntimerender",
        "Qt6",
        "Qt::Quick3DRuntimeRender",
        extra=["COMPONENTS", "Quick3DRuntimeRender"],
    ),
    LibraryMapping("quick3dutils", "Qt6", "Qt::Quick3DUtils", extra=["COMPONENTS", "Quick3DUtils"]),
    LibraryMapping(
        "quickcontrols2", "Qt6", "Qt::QuickControls2", extra=["COMPONENTS", "QuickControls2"]
    ),
    LibraryMapping("quick", "Qt6", "Qt::Quick", extra=["COMPONENTS", "Quick"]),
    LibraryMapping("quickshapes", "Qt6", "Qt::QuickShapes", extra=["COMPONENTS", "QuickShapes"]),
    LibraryMapping(
        "quicktemplates2", "Qt6", "Qt::QuickTemplates2", extra=["COMPONENTS", "QuickTemplates2"]
    ),
    LibraryMapping("quickwidgets", "Qt6", "Qt::QuickWidgets", extra=["COMPONENTS", "QuickWidgets"]),
    LibraryMapping("render", "Qt6", "Qt::3DRender", extra=["COMPONENTS", "3DRender"]),
    LibraryMapping(
        "remoteobjects", "Qt6", "Qt::RemoteObjects", extra=["COMPONENTS", "RemoteObjects"]
    ),
    LibraryMapping("script", "Qt6", "Qt::Script", extra=["COMPONENTS", "Script"]),
    LibraryMapping("scripttools", "Qt6", "Qt::ScriptTools", extra=["COMPONENTS", "ScriptTools"]),
    LibraryMapping("scxml", "Qt6", "Qt::Scxml", extra=["COMPONENTS", "Scxml"]),
    LibraryMapping("sensors", "Qt6", "Qt::Sensors", extra=["COMPONENTS", "Sensors"]),
    LibraryMapping("serialport", "Qt6", "Qt::SerialPort", extra=["COMPONENTS", "SerialPort"]),
    LibraryMapping("serialbus", "Qt6", "Qt::SerialBus", extra=["COMPONENTS", "SerialBus"]),
    LibraryMapping("services", "Qt6", "Qt::ServiceSupport", extra=["COMPONENTS", "ServiceSupport"]),
    LibraryMapping(
        "service_support", "Qt6", "Qt::ServiceSupport", extra=["COMPONENTS", "ServiceSupport"]
    ),
    LibraryMapping("shadertools", "Qt6", "Qt::ShaderTools", extra=["COMPONENTS", "ShaderTools"]),
    LibraryMapping("sql", "Qt6", "Qt::Sql", extra=["COMPONENTS", "Sql"]),
    LibraryMapping("svg", "Qt6", "Qt::Svg", extra=["COMPONENTS", "Svg"]),
    LibraryMapping("testlib", "Qt6", "Qt::Test", extra=["COMPONENTS", "Test"]),
    LibraryMapping("texttospeech", "Qt6", "Qt::TextToSpeech", extra=["COMPONENTS", "TextToSpeech"]),
    LibraryMapping(
        "theme_support", "Qt6", "Qt::ThemeSupport", extra=["COMPONENTS", "ThemeSupport"]
    ),
    LibraryMapping("tts", "Qt6", "Qt::TextToSpeech", extra=["COMPONENTS", "TextToSpeech"]),
    LibraryMapping("uiplugin", "Qt6", "Qt::UiPlugin", extra=["COMPONENTS", "UiPlugin"]),
    LibraryMapping("uitools", "Qt6", "Qt::UiTools", extra=["COMPONENTS", "UiTools"]),
    LibraryMapping(
        "virtualkeyboard", "Qt6", "Qt::VirtualKeyboard", extra=["COMPONENTS", "VirtualKeyboard"]
    ),
    LibraryMapping(
        "vulkan_support", "Qt6", "Qt::VulkanSupport", extra=["COMPONENTS", "VulkanSupport"]
    ),
    LibraryMapping(
        "waylandclient", "Qt6", "Qt::WaylandClient", extra=["COMPONENTS", "WaylandClient"]
    ),
    LibraryMapping(
        "waylandcompositor",
        "Qt6",
        "Qt::WaylandCompositor",
        extra=["COMPONENTS", "WaylandCompositor"],
    ),
    LibraryMapping("webchannel", "Qt6", "Qt::WebChannel", extra=["COMPONENTS", "WebChannel"]),
    LibraryMapping("webengine", "Qt6", "Qt::WebEngine", extra=["COMPONENTS", "WebEngine"]),
    LibraryMapping(
        "webenginewidgets", "Qt6", "Qt::WebEngineWidgets", extra=["COMPONENTS", "WebEngineWidgets"]
    ),
    LibraryMapping("websockets", "Qt6", "Qt::WebSockets", extra=["COMPONENTS", "WebSockets"]),
    LibraryMapping("webview", "Qt6", "Qt::WebView", extra=["COMPONENTS", "WebView"]),
    LibraryMapping("widgets", "Qt6", "Qt::Widgets", extra=["COMPONENTS", "Widgets"]),
    LibraryMapping("window-lib", "Qt6", "Qt::AppManWindow", extra=["COMPONENTS", "AppManWindow"]),
    LibraryMapping(
        "windowsuiautomation_support",
        "Qt6",
        "Qt::WindowsUIAutomationSupport",
        extra=["COMPONENTS", "WindowsUIAutomationSupport"],
    ),
    LibraryMapping("winextras", "Qt6", "Qt::WinExtras", extra=["COMPONENTS", "WinExtras"]),
    LibraryMapping("x11extras", "Qt6", "Qt::X11Extras", extra=["COMPONENTS", "X11Extras"]),
    LibraryMapping("xcb_qpa_lib", "Qt6", "Qt::XcbQpa", extra=["COMPONENTS", "XcbQpa"]),
    LibraryMapping(
        "xkbcommon_support", "Qt6", "Qt::XkbCommonSupport", extra=["COMPONENTS", "XkbCommonSupport"]
    ),
    LibraryMapping("xmlpatterns", "Qt6", "Qt::XmlPatterns", extra=["COMPONENTS", "XmlPatterns"]),
    LibraryMapping("xml", "Qt6", "Qt::Xml", extra=["COMPONENTS", "Xml"]),
    LibraryMapping(
        "qmlworkerscript", "Qt6", "Qt::QmlWorkerScript", extra=["COMPONENTS", "QmlWorkerScript"]
    ),
    LibraryMapping(
        "quickparticles", "Qt6", "Qt::QuickParticles", extra=["COMPONENTS", "QuickParticles"]
    ),
    LibraryMapping(
        "linuxofono_support",
        "Qt6",
        "Qt::LinuxOfonoSupport",
        extra=["COMPONENTS", "LinuxOfonoSupport"],
    ),
    LibraryMapping(
        "linuxofono_support_private",
        "Qt6",
        "Qt::LinuxOfonoSupportPrivate",
        extra=["COMPONENTS", "LinuxOfonoSupportPrivate"],
    ),
    LibraryMapping("tools", "Qt6", "Qt::Tools", extra=["COMPONENTS", "Tools"]),
    LibraryMapping("axcontainer", "Qt6", "Qt::AxContainer", extra=["COMPONENTS", "AxContainer"]),
    LibraryMapping(
        "webkitwidgets", "Qt6", "Qt::WebKitWidgets", extra=["COMPONENTS", "WebKitWidgets"]
    )
    # qtzlib: No longer supported.
]

# Note that the library map is adjusted dynamically further down.
_library_map = [
    # 3rd party:
    LibraryMapping("atspi", "ATSPI2", "PkgConfig::ATSPI2"),
    LibraryMapping("bluez", "BlueZ", "PkgConfig::BlueZ"),
    LibraryMapping("corewlan", None, None),
    LibraryMapping("cups", "Cups", "Cups::Cups"),
    LibraryMapping("directfb", "DirectFB", "PkgConfig::DirectFB"),
    LibraryMapping("db2", "DB2", "DB2::DB2"),
    LibraryMapping("dbus", "WrapDBus1", "dbus-1", resultVariable="DBus1"),
    LibraryMapping("doubleconversion", None, None),
    LibraryMapping("drm", "Libdrm", "Libdrm::Libdrm"),
    LibraryMapping("egl", "EGL", "EGL::EGL"),
    LibraryMapping("flite", "Flite", "Flite::Flite"),
    LibraryMapping("flite_alsa", "ALSA", "ALSA::ALSA"),
    LibraryMapping(
        "fontconfig", "Fontconfig", "Fontconfig::Fontconfig", resultVariable="FONTCONFIG"
    ),
    LibraryMapping("freetype", "WrapFreetype", "WrapFreetype::WrapFreetype", extra=["REQUIRED"]),
    LibraryMapping("gbm", "gbm", "gbm::gbm"),
    LibraryMapping("glib", "GLIB2", "GLIB2::GLIB2"),
    LibraryMapping("gnu_iconv", None, None),
    LibraryMapping("gtk3", "GTK3", "PkgConfig::GTK3"),
    LibraryMapping("gssapi", "GSSAPI", "GSSAPI::GSSAPI"),
    LibraryMapping("harfbuzz", "WrapHarfbuzz", "WrapHarfbuzz::WrapHarfbuzz"),
    LibraryMapping("host_dbus", None, None),
    LibraryMapping(
        "icu", "ICU", "ICU::i18n ICU::uc ICU::data", extra=["COMPONENTS", "i18n", "uc", "data"]
    ),
    LibraryMapping("journald", "Libsystemd", "PkgConfig::Libsystemd"),
    LibraryMapping("jpeg", "JPEG", "JPEG::JPEG"),  # see also libjpeg
    LibraryMapping("libatomic", "Atomic", "Atomic"),
    LibraryMapping("libclang", "WrapLibClang", "WrapLibClang::WrapLibClang"),
    LibraryMapping("libdl", None, "${CMAKE_DL_LIBS}"),
    LibraryMapping("libinput", "Libinput", "Libinput::Libinput"),
    LibraryMapping("libjpeg", "JPEG", "JPEG::JPEG"),  # see also jpeg
    LibraryMapping("libpng", "PNG", "PNG::PNG"),
    LibraryMapping("libproxy", "Libproxy", "PkgConfig::Libproxy"),
    LibraryMapping("librt", "WrapRt", "WrapRt"),
    LibraryMapping("libudev", "Libudev", "PkgConfig::Libudev"),
    LibraryMapping("lttng-ust", "LTTngUST", "LTTng::UST", resultVariable="LTTNGUST"),
    LibraryMapping("mtdev", "Mtdev", "PkgConfig::Mtdev"),
    LibraryMapping("mysql", "MySQL", "MySQL::MySQL"),
    LibraryMapping("odbc", "ODBC", "ODBC::ODBC"),
    LibraryMapping("opengl_es2", "GLESv2", "GLESv2::GLESv2"),
    LibraryMapping("opengl", "OpenGL", "OpenGL::GL", resultVariable="OpenGL_OpenGL"),
    LibraryMapping(
        "openssl_headers",
        "OpenSSL",
        "OpenSSL::SSL_nolink",
        resultVariable="OPENSSL_INCLUDE_DIR",
        appendFoundSuffix=False,
    ),
    LibraryMapping("openssl", "OpenSSL", "OpenSSL::SSL"),
    LibraryMapping("oci", "Oracle", "Oracle::OCI"),
    LibraryMapping("pcre2", "WrapPCRE2", "WrapPCRE2::WrapPCRE2", extra=["REQUIRED"]),
    LibraryMapping("posix_iconv", None, None),
    LibraryMapping("pps", "PPS", "PPS::PPS"),
    LibraryMapping("psql", "PostgreSQL", "PostgreSQL::PostgreSQL"),
    LibraryMapping("slog2", "Slog2", "Slog2::Slog2"),
    LibraryMapping("speechd", "SpeechDispatcher", "SpeechDispatcher::SpeechDispatcher"),
    LibraryMapping("sqlite2", None, None),  # No more sqlite2 support in Qt6!
    LibraryMapping("sqlite3", "SQLite3", "SQLite::SQLite3"),
    LibraryMapping("sun_iconv", None, None),
    LibraryMapping("tslib", "Tslib", "PkgConfig::Tslib"),
    LibraryMapping("udev", "Libudev", "PkgConfig::Libudev"),
    LibraryMapping("udev", "Libudev", "PkgConfig::Libudev"),  # see also libudev!
    LibraryMapping("vulkan", "Vulkan", "Vulkan::Vulkan"),
    LibraryMapping("wayland_server", "Wayland", "Wayland::Server"),  # used in qtbase/src/gui
    LibraryMapping("wayland-server", "Wayland", "Wayland::Server"),  # used in qtwayland
    LibraryMapping("wayland-client", "Wayland", "Wayland::Client"),
    LibraryMapping("wayland-cursor", "Wayland", "Wayland::Cursor"),
    LibraryMapping("wayland-egl", "Wayland", "Wayland::Egl"),
    LibraryMapping(
        "wayland-kms", "Waylandkms", "PkgConfig::Waylandkms"
    ),  # TODO: check if this actually works
    LibraryMapping("x11", "X11", "X11::X11"),
    LibraryMapping("x11sm", "X11", "${X11_SM_LIB} ${X11_ICE_LIB}", resultVariable="X11_SM"),
    LibraryMapping(
        "xcb",
        "XCB",
        "XCB::XCB",
        extra=["1.9"],
        resultVariable="TARGET XCB::XCB",
        appendFoundSuffix=False,
    ),
    LibraryMapping(
        "xcb_glx", "XCB", "XCB::GLX", extra=["COMPONENTS", "GLX"], resultVariable="XCB_GLX"
    ),
    LibraryMapping(
        "xcb_icccm", "XCB", "XCB::ICCCM", extra=["COMPONENTS", "ICCCM"], resultVariable="XCB_ICCCM"
    ),
    LibraryMapping(
        "xcb_image", "XCB", "XCB::IMAGE", extra=["COMPONENTS", "IMAGE"], resultVariable="XCB_IMAGE"
    ),
    LibraryMapping(
        "xcb_keysyms",
        "XCB",
        "XCB::KEYSYMS",
        extra=["COMPONENTS", "KEYSYMS"],
        resultVariable="XCB_KEYSYMS",
    ),
    LibraryMapping(
        "xcb_randr", "XCB", "XCB::RANDR", extra=["COMPONENTS", "RANDR"], resultVariable="XCB_RANDR"
    ),
    LibraryMapping(
        "xcb_render",
        "XCB",
        "XCB::RENDER",
        extra=["COMPONENTS", "RENDER"],
        resultVariable="XCB_RENDER",
    ),
    LibraryMapping(
        "xcb_renderutil",
        "XCB",
        "XCB::RENDERUTIL",
        extra=["COMPONENTS", "RENDERUTIL"],
        resultVariable="XCB_RENDERUTIL",
    ),
    LibraryMapping(
        "xcb_shape", "XCB", "XCB::SHAPE", extra=["COMPONENTS", "SHAPE"], resultVariable="XCB_SHAPE"
    ),
    LibraryMapping(
        "xcb_shm", "XCB", "XCB::SHM", extra=["COMPONENTS", "SHM"], resultVariable="XCB_SHM"
    ),
    LibraryMapping(
        "xcb_sync", "XCB", "XCB::SYNC", extra=["COMPONENTS", "SYNC"], resultVariable="XCB_SYNC"
    ),
    LibraryMapping(
        "xcb_xfixes",
        "XCB",
        "XCB::XFIXES",
        extra=["COMPONENTS", "XFIXES"],
        resultVariable="XCB_XFIXES",
    ),
    LibraryMapping(
        "xcb_xinerama",
        "XCB",
        "XCB::XINERAMA",
        extra=["COMPONENTS", "XINERAMA"],
        resultVariable="XCB_XINERAMA",
    ),
    LibraryMapping(
        "xcb_xinput",
        "XCB",
        "XCB::XINPUT",
        extra=["COMPONENTS", "XINPUT"],
        resultVariable="XCB_XINPUT",
    ),
    LibraryMapping(
        "xcb_xkb", "XCB", "XCB::XKB", extra=["COMPONENTS", "XKB"], resultVariable="XCB_XKB"
    ),
    LibraryMapping("xcb_xlib", "X11_XCB", "X11::XCB"),
    LibraryMapping("xcomposite", "XComposite", "PkgConfig::XComposite"),
    LibraryMapping("xkbcommon_evdev", "XKB", "XKB::XKB", extra=["0.4.1"]),  # see also xkbcommon
    LibraryMapping("xkbcommon_x11", "XKB", "XKB::XKB", extra=["0.4.1"]),  # see also xkbcommon
    LibraryMapping("xkbcommon", "XKB", "XKB::XKB", extra=["0.4.1"]),
    LibraryMapping("xlib", "X11", "X11::XCB"),  # FIXME: Is this correct?
    LibraryMapping("xrender", "XRender", "PkgConfig::XRender"),
    LibraryMapping("zlib", "ZLIB", "ZLIB::ZLIB", extra=["REQUIRED"]),
    LibraryMapping("zstd", "ZSTD", "ZSTD::ZSTD"),
    LibraryMapping("tiff", "TIFF", "TIFF::TIFF"),
    LibraryMapping("webp", "WrapWebP", "WrapWebP::WrapWebP"),
    LibraryMapping("jasper", "WrapJasper", "WrapJasper::WrapJasper"),
    LibraryMapping("sdl2", "WrapSDL2", "WrapSDL2::WrapSDL2"),
    LibraryMapping("assimp", "WrapAssimp", "WrapAssimp::WrapAssimp", extra=["5"]),
]


def _adjust_library_map():
    # Assign a Linux condition on all x and wayland related packages.
    # We don't want to get pages of package not found messages on
    # Windows and macOS, and this also improves configure time on
    # those platforms.
    linux_package_prefixes = ["xcb", "x11", "xkb", "xrender", "xlib", "wayland"]
    for i, _ in enumerate(_library_map):
        if any([_library_map[i].soName.startswith(p) for p in linux_package_prefixes]):
            _library_map[i].emit_if = "config.linux"


_adjust_library_map()


def find_3rd_party_library_mapping(soName: str) -> typing.Optional[LibraryMapping]:
    for i in _library_map:
        if i.soName == soName:
            return i
    return None


def find_qt_library_mapping(soName: str) -> typing.Optional[LibraryMapping]:
    for i in _qt_library_map:
        if i.soName == soName:
            return i
    return None


def find_library_info_for_target(targetName: str) -> typing.Optional[LibraryMapping]:
    qt_target = targetName
    if targetName.endswith("Private"):
        qt_target = qt_target[:-7]

    for i in _qt_library_map:
        if i.targetName == qt_target:
            return i

    for i in _library_map:
        if i.targetName == targetName:
            return i

    return None


def featureName(name: str) -> str:
    replacement_char = "_"
    if name.startswith("c++"):
        replacement_char = "x"
    return re.sub(r"[^a-zA-Z0-9_]", replacement_char, name)


def map_qt_library(lib: str) -> str:
    private = False
    if lib.endswith("-private"):
        private = True
        lib = lib[:-8]
    mapped = find_qt_library_mapping(lib)
    qt_name = lib
    if mapped:
        assert mapped.targetName  # Qt libs must have a target name set
        qt_name = mapped.targetName
    if private:
        qt_name += "Private"
    return qt_name


platform_mapping = {
    "win32": "WIN32",
    "win": "WIN32",
    "unix": "UNIX",
    "darwin": "APPLE",
    "linux": "LINUX",
    "integrity": "INTEGRITY",
    "qnx": "QNX",
    "vxworks": "VXWORKS",
    "hpux": "HPUX",
    "nacl": "NACL",
    "android": "ANDROID",
    "android-embedded": "ANDROID_EMBEDDED",
    "uikit": "APPLE_UIKIT",
    "tvos": "APPLE_TVOS",
    "watchos": "APPLE_WATCHOS",
    "winrt": "WINRT",
    "wasm": "WASM",
    "emscripten": "EMSCRIPTEN",
    "msvc": "MSVC",
    "clang": "CLANG",
    "gcc": "GCC",
    "icc": "ICC",
    "intel_icc": "ICC",
    "osx": "APPLE_OSX",
    "ios": "APPLE_IOS",
    "freebsd": "FREEBSD",
    "openbsd": "OPENBSD",
    "netbsd": "NETBSD",
    "haiku": "HAIKU",
    "netbsd": "NETBSD",
    "mac": "APPLE",
    "macx": "APPLE_OSX",
    "macos": "APPLE_OSX",
    "macx-icc": "(APPLE_OSX AND ICC)",
}


def map_platform(platform: str) -> str:
    """ Return the qmake platform as cmake platform or the unchanged string. """
    return platform_mapping.get(platform, platform)


def is_known_3rd_party_library(lib: str) -> bool:
    if lib.endswith("/nolink") or lib.endswith("_nolink"):
        lib = lib[:-7]
    mapping = find_3rd_party_library_mapping(lib)

    return mapping is not None


def map_3rd_party_library(lib: str) -> str:
    libpostfix = ""
    if lib.endswith("/nolink"):
        lib = lib[:-7]
        libpostfix = "_nolink"
    mapping = find_3rd_party_library_mapping(lib)
    if not mapping or not mapping.targetName:
        return lib
    return mapping.targetName + libpostfix


def generate_find_package_info(
    lib: LibraryMapping, use_qt_find_package: bool = True, *, indent: int = 0, emit_if: str = ""
) -> str:
    isRequired = False

    extra = lib.extra.copy()

    if "REQUIRED" in extra and use_qt_find_package:
        isRequired = True
        extra.remove("REQUIRED")

    cmake_target_name = lib.targetName
    assert cmake_target_name

    # _nolink or not does not matter at this point:
    if cmake_target_name.endswith("_nolink") or cmake_target_name.endswith("/nolink"):
        cmake_target_name = cmake_target_name[:-7]

    if cmake_target_name and use_qt_find_package:
        extra += ["PROVIDED_TARGETS", cmake_target_name]

    result = ""
    one_ind = "    "
    ind = one_ind * indent

    if use_qt_find_package:
        if extra:
            result = f"{ind}qt_find_package({lib.packageName} {' '.join(extra)})\n"
        else:
            result = f"{ind}qt_find_package({lib.packageName})\n"

        if isRequired:
            result += f"{ind}set_package_properties({lib.packageName} PROPERTIES TYPE REQUIRED)\n"
    else:
        if extra:
            result = f"{ind}find_package({lib.packageName} {' '.join(extra)})\n"
        else:
            result = f"{ind}find_package({lib.packageName})\n"

    # If a package should be found only in certain conditions, wrap
    # the find_package call within that condition.
    if emit_if:
        result = f"if(({emit_if}) OR QT_FIND_ALL_PACKAGES_ALWAYS)\n{one_ind}{result}endif()\n"

    return result


def _set_up_py_parsing_nicer_debug_output(pp):
    indent = -1

    def increase_indent(fn):
        def wrapper_function(*args):
            nonlocal indent
            indent += 1
            print("> " * indent, end="")
            return fn(*args)

        return wrapper_function

    def decrease_indent(fn):
        def wrapper_function(*args):
            nonlocal indent
            print("> " * indent, end="")
            indent -= 1
            return fn(*args)

        return wrapper_function

    pp._defaultStartDebugAction = increase_indent(pp._defaultStartDebugAction)
    pp._defaultSuccessDebugAction = decrease_indent(pp._defaultSuccessDebugAction)
    pp._defaultExceptionDebugAction = decrease_indent(pp._defaultExceptionDebugAction)
