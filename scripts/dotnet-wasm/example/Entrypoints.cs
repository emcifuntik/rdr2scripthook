using System;
using System.Runtime.InteropServices;
using Rdr2.Wasm;

namespace Rdr2.DotNet.Example;

public static class Entrypoints
{
    private static int _toggleBinding;
    private static bool _visible;
    private static bool _cursorHeld;

    private static ReadOnlySpan<byte> PageUrl => "data:text/html;base64,PCFkb2N0eXBlIGh0bWw+PGh0bWw+PGhlYWQ+PG1ldGEgY2hhcnNldD0idXRmLTgiPjxzdHlsZT4qe2JveC1zaXppbmc6Ym9yZGVyLWJveH1odG1sLGJvZHl7bWFyZ2luOjA7d2lkdGg6MTAwJTtoZWlnaHQ6MTAwJTtiYWNrZ3JvdW5kOnRyYW5zcGFyZW50O2NvbG9yOiNmOGZhZmM7Zm9udC1mYW1pbHk6U2Vnb2UgVUksc2Fucy1zZXJpZn1ib2R5e2Rpc3BsYXk6Z3JpZDtwbGFjZS1pdGVtczpjZW50ZXI7YmFja2dyb3VuZDpyYWRpYWwtZ3JhZGllbnQoY2lyY2xlIGF0IGNlbnRlcixyZ2JhKDE1LDIzLDQyLC4yNSkscmdiYSgyLDYsMjMsLjcyKSl9LmNhcmR7d2lkdGg6bWluKDcyMHB4LDgwdncpO3BhZGRpbmc6NDhweDtib3JkZXI6MXB4IHNvbGlkIHJnYmEoMTQ4LDE2MywxODQsLjM1KTtib3JkZXItcmFkaXVzOjI0cHg7YmFja2dyb3VuZDpyZ2JhKDE1LDIzLDQyLC45Myk7Ym94LXNoYWRvdzowIDMwcHggOTBweCAjMDAwOX1oMXttYXJnaW46MCAwIDEycHg7Zm9udC1zaXplOjQycHg7Y29sb3I6IzdkZDNmY31we2ZvbnQtc2l6ZToyMHB4O2xpbmUtaGVpZ2h0OjEuNTU7Y29sb3I6I2NiZDVlMX0udGFne2Rpc3BsYXk6aW5saW5lLWJsb2NrO3BhZGRpbmc6OHB4IDE0cHg7Ym9yZGVyLXJhZGl1czo5OTlweDtiYWNrZ3JvdW5kOiMwYzRhNmU7Y29sb3I6I2JhZTZmZDtmb250LXdlaWdodDo3MDB9a2Jke3BhZGRpbmc6NHB4IDlweDtib3JkZXItcmFkaXVzOjZweDtiYWNrZ3JvdW5kOiNlMmU4ZjA7Y29sb3I6IzBmMTcyYTtmb250LXdlaWdodDo4MDB9PC9zdHlsZT48L2hlYWQ+PGJvZHk+PG1haW4gY2xhc3M9ImNhcmQiPjxzcGFuIGNsYXNzPSJ0YWciPldBU01USU1FIFNBTkRCT1g8L3NwYW4+PGgxPi5ORVQgTmF0aXZlQU9UIGlzIHJ1bm5pbmc8L2gxPjxwPlRoaXMgV2ViVmlldyB3YXMgb3BlbmVkIGJ5IEMjIGNvbXBpbGVkIGFoZWFkIG9mIHRpbWUgaW50byBhIFdBU0kgUHJldmlldyAxIGNvcmUgbW9kdWxlLiBObyBNb25vLCBDb3JlQ0xSLCBob3N0ZnhyLCBvciBtYW5hZ2VkIEpJVCBpcyBsb2FkZWQgaW4gdGhlIGdhbWUuPC9wPjxwPlByZXNzIDxrYmQ+RjU8L2tiZD4gYWdhaW4gdG8gY2xvc2UgaXQuPC9wPjwvbWFpbj48L2JvZHk+PC9odG1sPg=="u8;

    [UnmanagedCallersOnly(EntryPoint = "rdr2_abi_version")]
    public static uint AbiVersion() => Host.AbiVersion;

    [UnmanagedCallersOnly(EntryPoint = "rdr2_init")]
    public static void Initialize()
    {
        Host.Log(".NET NativeAOT guest initialized inside Wasmtime"u8);
        _toggleBinding = Host.RegisterBinding(
            "toggle_dotnet_example"u8,
            "Toggle .NET Wasmtime example"u8,
            "keyboard"u8,
            "F5"u8);
    }

    [UnmanagedCallersOnly(EntryPoint = "rdr2_tick")]
    public static void Tick()
    {
        if (_toggleBinding <= 0) return;
        if ((BindingEvent)Host.PollBindingEvent(_toggleBinding) ==
            BindingEvent.Down)
            ToggleWebView();
    }

    [UnmanagedCallersOnly(EntryPoint = "rdr2_key_down")]
    public static void KeyDown(uint virtualKey) { }

    [UnmanagedCallersOnly(EntryPoint = "rdr2_key_up")]
    public static void KeyUp(uint virtualKey) { }

    [UnmanagedCallersOnly(EntryPoint = "rdr2_shutdown")]
    public static void Shutdown()
    {
        HideWebView();
        if (_toggleBinding > 0)
            Host.UnregisterBinding(_toggleBinding);
        _toggleBinding = 0;
        Host.Log(".NET NativeAOT guest shutting down"u8);
    }

    private static void ToggleWebView()
    {
        if (_visible)
        {
            HideWebView();
            return;
        }

        int status = Host.OpenWebView(PageUrl);
        if (status != 0)
        {
            Host.Log("Could not open the .NET WebView"u8, LogLevel.Error);
            return;
        }

        status = Host.SetWebViewFocus(true);
        if (status != 0)
        {
            Host.SetWebViewVisible(false);
            Host.Log("Could not focus the .NET WebView"u8, LogLevel.Error);
            return;
        }

        status = Host.ShowCursor();
        if (status != 0)
        {
            Host.SetWebViewFocus(false);
            Host.SetWebViewVisible(false);
            Host.Log("Could not show the .NET WebView cursor"u8,
                     LogLevel.Error);
            return;
        }

        _cursorHeld = true;
        _visible = true;
        Host.Log(".NET WebView opened; press F5 to close"u8);
    }

    private static void HideWebView()
    {
        if (!_visible && !_cursorHeld) return;

        Host.SetWebViewFocus(false);
        if (_cursorHeld)
        {
            Host.HideCursor();
            _cursorHeld = false;
        }
        Host.SetWebViewVisible(false);
        _visible = false;
        Host.Log(".NET WebView hidden"u8);
    }
}
