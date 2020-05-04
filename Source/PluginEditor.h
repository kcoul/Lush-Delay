/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "HeaderPanel.h"
#include "ControlPanel.h"
#include "LushLookAndFeel.h"

//==============================================================================
/**
*/
class LushDelayAudioProcessorEditor  : public AudioProcessorEditor, public Button::Listener
{
public:
    LushDelayAudioProcessorEditor (LushDelayAudioProcessor&);
    ~LushDelayAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void buttonClicked (Button *button) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LushDelayAudioProcessor& processor;
    HeaderPanel headerPanel;
    ControlPanel controlPanel;
    stm::DebugDisplay debugDisplay;
    stm::DisabledVeil bypassedVeil;
    const float headerRatio = 0.43f;
    const float whRatio = 0.59f;
    const int width = 450;
    const int debugWidth = 200;

    LushLookAndFeel lushLookAndFeel;
    
    const bool debug = true;
    
    TextButton matchedBypassButton;
    typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    std::unique_ptr<ButtonAttachment> matchedBypassButtonAttachment;
    
    void updateWindow();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LushDelayAudioProcessorEditor)
};
