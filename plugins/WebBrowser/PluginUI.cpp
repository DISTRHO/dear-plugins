/*
 * Web Browser example
 * Copyright (C) 2025 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: ISC
 */

#include "DistrhoUI.hpp"

#include "extra/WebView.hpp"

#include <memory>

START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

class WebBrowserUI : public UI
{
    bool urlChanged = false;
    bool urlFirstInit = true;
    bool urlNeedsReload = false;
    char urlLabelBuffer[0xff] = "https://github.com/DISTRHO/dear-plugins";
    uint urlLabelHeight = 0;
    std::unique_ptr<WebViewData, WebViewDestroy> webview;

public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */
    WebBrowserUI()
        : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT)
    {
        const double scaleFactor = getScaleFactor();

        if (d_isEqual(scaleFactor, 1.0))
        {
            urlLabelHeight = 36;
            setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT);
        }
        else
        {
            const uint width = DISTRHO_UI_DEFAULT_WIDTH * scaleFactor;
            const uint height = DISTRHO_UI_DEFAULT_HEIGHT * scaleFactor;
            urlLabelHeight = 36 * scaleFactor;
            setGeometryConstraints(width, height);
            setSize(width, height);
        }

        setFontSize(16);
    }

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

   /**
      A parameter has changed on the plugin side.@n
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t, float) override {}

   /**
      A state has changed on the plugin side.@n
      This is called by the host to inform the UI about state changes.
    */
    void stateChanged(const char* const key, const char* const value) override
    {
        if (std::strcmp(key, "url") != 0)
            return;

        if (urlLabelBuffer != value)
        {
            std::strncpy(urlLabelBuffer, value, sizeof(urlLabelBuffer));
            urlLabelBuffer[sizeof(urlLabelBuffer) - 1] = '\0';
        }

        fprintf(stderr, "url changed to %s\n", value);

        const double scaleFactor = getScaleFactor();

        WebViewOptions opts;
        opts.offset.y = urlLabelHeight;

        webview = std::unique_ptr<WebViewData, WebViewDestroy>(
            webViewCreate(value,
                          getWindow().getNativeWindowHandle(),
                          getWidth(),
                          getHeight() - urlLabelHeight,
                          scaleFactor,
                          opts));
    }

    // ----------------------------------------------------------------------------------------------------------------
    // Widget Callbacks

   /**
      ImGui specific onDisplay function.
    */
    void onImGuiDisplay() override
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(getWidth(), urlLabelHeight));
        ImGui::SetNextWindowContentSize(ImVec2(getWidth(), urlLabelHeight));

        constexpr const ImGuiWindowFlags windowFlags = 0
            |ImGuiWindowFlags_NoDecoration
            |ImGuiWindowFlags_NoMove
            |ImGuiWindowFlags_NoScrollWithMouse
            |ImGuiWindowFlags_AlwaysAutoResize;

        constexpr const ImGuiInputTextFlags textFlags = 0
            |ImGuiInputTextFlags_CharsNoBlank
            |ImGuiInputTextFlags_AutoSelectAll
            |ImGuiInputTextFlags_EnterReturnsTrue;

        if (ImGui::Begin("WebBrowserURL", nullptr, windowFlags))
        {
            ImGui::BeginGroup();

            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("URL:");
            ImGui::SameLine();

            ImGui::SetNextItemWidth(-76 * getScaleFactor());
            if (ImGui::InputText("", urlLabelBuffer, sizeof(urlLabelBuffer) - 8, textFlags))
            {
                urlChanged = true;

                // add http:// suffix is no protocol is defined in newly changed URL
                if (std::strchr(urlLabelBuffer, ':') == nullptr)
                {
                    std::memmove(urlLabelBuffer + 7, urlLabelBuffer, std::strlen(urlLabelBuffer) + 1);
                    urlLabelBuffer[sizeof(urlLabelBuffer) - 1] = '\0';
                    std::memcpy(urlLabelBuffer, "http://", 7);
                }
            }
            ImGui::SameLine();

            if (ImGui::Button("Reload"))
                urlNeedsReload = true;

            ImGui::EndGroup();
        }

        ImGui::End();
    }

    // ----------------------------------------------------------------------------------------------------------------
    // UI Callbacks

   /**
      UI idle function, called to give idle time to the plugin UI directly from the host.
      This is called right after OS event handling and Window idle events (within the same cycle).
      There are no guarantees in terms of timing.
    */
    void uiIdle() override
    {
        if (urlChanged || urlFirstInit)
        {
            // change our webview contents
            stateChanged("url", urlLabelBuffer);

            if (urlFirstInit)
            {
                // inform host of the state change
                setState("url", urlLabelBuffer);
                urlFirstInit = false;
            }

            urlChanged = false;
            urlNeedsReload = false;
        }

        if (urlNeedsReload)
        {
            urlNeedsReload = false;

            if (const WebViewHandle handle = webview.get())
                webViewReload(handle);
        }
        else
        {
            if (const WebViewHandle handle = webview.get())
                webViewIdle(handle);
        }
    }

   /**
      Widget resize function, called when the widget is resized.
      This is overriden here so the host knows when the UI is resized by you.
    */
    void onResize(const ResizeEvent& ev) override
    {
        UI::onResize(ev);

        if (const WebViewHandle handle = webview.get())
            webViewResize(handle, ev.size.getWidth(), ev.size.getHeight() - urlLabelHeight, getScaleFactor());
    }

    // ----------------------------------------------------------------------------------------------------------------

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WebBrowserUI)
};

// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new WebBrowserUI();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
