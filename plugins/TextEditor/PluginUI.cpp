/*
 * Text Editor example
 * Copyright (C) 2023-2025 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: ISC
 */

#include "DistrhoUI.hpp"

#include <fstream>

START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

class TextEditorUI : public UI
{
    bool textChanged = false;

public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */
    TextEditorUI()
        : UI()
    {
        const double scaleFactor = getScaleFactor();
        setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH * scaleFactor, DISTRHO_UI_DEFAULT_HEIGHT * scaleFactor);

        showMenu();
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
    void stateChanged(const char* key, const char* value) override
    {
        if (std::strcmp(key, "text") != 0)
            return;

        setText(value);
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
        if (hasTextChangedSinceLastTime())
        {
            textChanged = true;
            // wait for first idle with no more text changes
            return;
        }

        if (textChanged)
        {
            textChanged = false;
            setState("text", getText().c_str());
        }
    }

   /**
      Window file selected function, called when a path is selected by the user, as triggered by openFileBrowser().
      This function is for plugin UIs to be able to override Window::onFileSelected(const char*).

      This action happens after the user confirms the action, so the file browser dialog will be closed at this point.
      The default implementation does nothing.

      If you need to use files as plugin state, please setup and use states with kStateIsFilenamePath instead.
    */
    void uiFileBrowserSelected(const char* const filename) override
    {
        // TODO check if this can be null, add to docs if that can happen
        if (filename == nullptr)
            return;

        std::ifstream filestream(filename);

        if (filestream.good())
        {
            const std::string text((std::istreambuf_iterator<char>(filestream)), std::istreambuf_iterator<char>());
            setText(text);
            setState("text", text.c_str());
        }
    }

    // ----------------------------------------------------------------------------------------------------------------

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditorUI)
};

// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new TextEditorUI();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
