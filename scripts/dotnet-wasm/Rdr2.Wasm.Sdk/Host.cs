using System.Runtime.InteropServices;

namespace Rdr2.Wasm;

public static unsafe class Host
{
    public const uint AbiVersion = 1;

    public static void Log(ReadOnlySpan<byte> utf8, LogLevel level = LogLevel.Info)
    {
        fixed (byte* pointer = utf8)
            Native.Log((int)level, pointer, utf8.Length);
    }

    public static uint Joaat(ReadOnlySpan<byte> utf8)
    {
        fixed (byte* pointer = utf8)
            return Native.Joaat(pointer, utf8.Length);
    }

    public static uint GameTime => Native.GameTime();

    public static bool IsKeyPressed(uint virtualKey) =>
        Native.IsKeyPressed(virtualKey) != 0;

    public static bool IsKeyJustPressed(uint virtualKey) =>
        Native.IsKeyJustPressed(virtualKey) != 0;

    public static int OpenWebView(ReadOnlySpan<byte> url, int width = 0,
                                  int height = 0)
    {
        fixed (byte* pointer = url)
            return Native.WebViewOpen(pointer, url.Length, width, height);
    }

    public static void CloseWebView() => Native.WebViewClose();
    public static bool IsWebViewOpen => Native.WebViewIsOpen() != 0;
    public static int SetWebViewVisible(bool visible) =>
        Native.WebViewSetVisible(visible ? 1 : 0);
    public static bool IsWebViewVisible => Native.WebViewIsVisible() != 0;
    public static int SetWebViewFocus(bool focused) =>
        Native.WebViewSetFocus(focused ? 1 : 0);
    public static bool IsWebViewFocused => Native.WebViewIsFocused() != 0;
    public static int ShowCursor() => Native.WebViewShowCursor();
    public static int HideCursor() => Native.WebViewHideCursor();
    public static bool IsCursorVisible => Native.WebViewIsCursorVisible() != 0;
    public static int CursorReferenceCount => Native.WebViewCursorRefCount();

    public static int NavigateWebView(ReadOnlySpan<byte> url)
    {
        fixed (byte* pointer = url)
            return Native.WebViewNavigate(pointer, url.Length);
    }

    public static int PostWebViewJson(ReadOnlySpan<byte> json)
    {
        fixed (byte* pointer = json)
            return Native.WebViewPostJson(pointer, json.Length);
    }

    public static int PollWebViewJson(Span<byte> destination)
    {
        fixed (byte* pointer = destination)
            return Native.WebViewPollJson(pointer, destination.Length);
    }

    public static int RegisterBinding(ReadOnlySpan<byte> id,
                                      ReadOnlySpan<byte> description,
                                      ReadOnlySpan<byte> mapper,
                                      ReadOnlySpan<byte> defaultParameter)
    {
        fixed (byte* idPointer = id)
        fixed (byte* descriptionPointer = description)
        fixed (byte* mapperPointer = mapper)
        fixed (byte* parameterPointer = defaultParameter)
            return Native.InputRegisterBinding(
                idPointer, id.Length,
                descriptionPointer, description.Length,
                mapperPointer, mapper.Length,
                parameterPointer, defaultParameter.Length);
    }

    public static int PollBindingEvent(int handle) =>
        Native.InputPollBindingEvent(handle);

    public static int UnregisterBinding(int handle) =>
        Native.InputUnregisterBinding(handle);

    public static bool IsBindingDown(int handle) =>
        Native.InputIsBindingDown(handle) != 0;

    public static int GetBindingParameter(int handle, Span<byte> destination)
    {
        fixed (byte* pointer = destination)
            return Native.InputGetBindingParameter(handle, pointer,
                                                   destination.Length);
    }

    public static int SetBinding(int handle, ReadOnlySpan<byte> mapper,
                                 ReadOnlySpan<byte> parameter)
    {
        fixed (byte* mapperPointer = mapper)
        fixed (byte* parameterPointer = parameter)
            return Native.InputSetBinding(handle, mapperPointer, mapper.Length,
                                          parameterPointer, parameter.Length);
    }

    public static int ResetBinding(int handle) =>
        Native.InputResetBinding(handle);

    private static class Native
    {
        [DllImport("rdr2", EntryPoint = "log")]
        [WasmImportLinkage]
        internal static extern void Log(int level, byte* pointer, int length);

        [DllImport("rdr2", EntryPoint = "joaat")]
        [WasmImportLinkage]
        internal static extern uint Joaat(byte* pointer, int length);

        [DllImport("rdr2", EntryPoint = "game_time")]
        [WasmImportLinkage]
        internal static extern uint GameTime();

        [DllImport("rdr2", EntryPoint = "is_key_pressed")]
        [WasmImportLinkage]
        internal static extern int IsKeyPressed(uint virtualKey);

        [DllImport("rdr2", EntryPoint = "is_key_just_pressed")]
        [WasmImportLinkage]
        internal static extern int IsKeyJustPressed(uint virtualKey);

        [DllImport("rdr2", EntryPoint = "input_register_binding")]
        [WasmImportLinkage]
        internal static extern int InputRegisterBinding(
            byte* idPointer, int idLength,
            byte* descriptionPointer, int descriptionLength,
            byte* mapperPointer, int mapperLength,
            byte* parameterPointer, int parameterLength);

        [DllImport("rdr2", EntryPoint = "input_poll_binding_event")]
        [WasmImportLinkage]
        internal static extern int InputPollBindingEvent(int handle);

        [DllImport("rdr2", EntryPoint = "input_unregister_binding")]
        [WasmImportLinkage]
        internal static extern int InputUnregisterBinding(int handle);

        [DllImport("rdr2", EntryPoint = "input_is_binding_down")]
        [WasmImportLinkage]
        internal static extern int InputIsBindingDown(int handle);

        [DllImport("rdr2", EntryPoint = "input_get_binding_parameter")]
        [WasmImportLinkage]
        internal static extern int InputGetBindingParameter(
            int handle, byte* pointer, int capacity);

        [DllImport("rdr2", EntryPoint = "input_set_binding")]
        [WasmImportLinkage]
        internal static extern int InputSetBinding(
            int handle, byte* mapperPointer, int mapperLength,
            byte* parameterPointer, int parameterLength);

        [DllImport("rdr2", EntryPoint = "input_reset_binding")]
        [WasmImportLinkage]
        internal static extern int InputResetBinding(int handle);

        [DllImport("rdr2", EntryPoint = "webview_open")]
        [WasmImportLinkage]
        internal static extern int WebViewOpen(byte* pointer, int length,
                                               int width, int height);

        [DllImport("rdr2", EntryPoint = "webview_close")]
        [WasmImportLinkage]
        internal static extern void WebViewClose();

        [DllImport("rdr2", EntryPoint = "webview_is_open")]
        [WasmImportLinkage]
        internal static extern int WebViewIsOpen();

        [DllImport("rdr2", EntryPoint = "webview_set_visible")]
        [WasmImportLinkage]
        internal static extern int WebViewSetVisible(int visible);

        [DllImport("rdr2", EntryPoint = "webview_is_visible")]
        [WasmImportLinkage]
        internal static extern int WebViewIsVisible();

        [DllImport("rdr2", EntryPoint = "webview_set_focus")]
        [WasmImportLinkage]
        internal static extern int WebViewSetFocus(int focused);

        [DllImport("rdr2", EntryPoint = "webview_is_focused")]
        [WasmImportLinkage]
        internal static extern int WebViewIsFocused();

        [DllImport("rdr2", EntryPoint = "webview_show_cursor")]
        [WasmImportLinkage]
        internal static extern int WebViewShowCursor();

        [DllImport("rdr2", EntryPoint = "webview_hide_cursor")]
        [WasmImportLinkage]
        internal static extern int WebViewHideCursor();

        [DllImport("rdr2", EntryPoint = "webview_is_cursor_visible")]
        [WasmImportLinkage]
        internal static extern int WebViewIsCursorVisible();

        [DllImport("rdr2", EntryPoint = "webview_cursor_ref_count")]
        [WasmImportLinkage]
        internal static extern int WebViewCursorRefCount();

        [DllImport("rdr2", EntryPoint = "webview_navigate")]
        [WasmImportLinkage]
        internal static extern int WebViewNavigate(byte* pointer, int length);

        [DllImport("rdr2", EntryPoint = "webview_post_json")]
        [WasmImportLinkage]
        internal static extern int WebViewPostJson(byte* pointer, int length);

        [DllImport("rdr2", EntryPoint = "webview_poll_json")]
        [WasmImportLinkage]
        internal static extern int WebViewPollJson(byte* pointer, int capacity);
    }
}

public enum LogLevel
{
    Info = 0,
    Warning = 1,
    Error = 2,
    Debug = 3,
}

public enum BindingEvent
{
    None = 0,
    Down = 1,
    Up = 2,
}
