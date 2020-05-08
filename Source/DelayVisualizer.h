/*
  ==============================================================================

    DelayVisualizer.h
    Created: 4 May 2020 10:14:38am
    Author:  Spenser Saling

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "LushLookAndFeel.h"
#include "Params.h"

//==============================================================================
/*
*/
class DelayVisualizer    : public Component, public stm::ParameterAttachment::Listener
{
public:
    DelayVisualizer(AudioProcessorValueTreeState& s)
        : crossFBAttachment(s, Params::idFeedbackCross)
        , directFBAttachment(s, Params::idFeedbackDirect)
        , delayAttachment(s, Params::idDelay)
        , offsetAttachment(s, Params::idOffsetLR)
        , panAttachment(s, Params::idPan)
    {
        crossFBAttachment.setListener(this);
        directFBAttachment.setListener(this);
        delayAttachment.setListener(this);
        offsetAttachment.setListener(this);
        panAttachment.setListener(this);
        
        crossFBsvg = Drawable::createFromImageData(BinaryData::VisualizerCrossFB_svg, BinaryData::VisualizerCrossFB_svgSize);
        directFBsvg = Drawable::createFromImageData(BinaryData::VisualizerDirectFB_svg, BinaryData::VisualizerDirectFB_svgSize);
        leftsvg = Drawable::createFromImageData(BinaryData::VisualizerLeft_svg, BinaryData::VisualizerLeft_svgSize);
        rightsvg = Drawable::createFromImageData(BinaryData::VisualizerRight_svg, BinaryData::VisualizerRight_svgSize);

        addAndMakeVisible(*crossFBsvg);
        addAndMakeVisible(*directFBsvg);
        addAndMakeVisible(*leftsvg);
        addAndMakeVisible(*rightsvg);
        
        initLabel(leftDelayLabel, "");
        initLabel(rightDelayLabel, "");
    }

    ~DelayVisualizer()
    {
        
    }

    void paint (Graphics& g) override
    {
        g.setColour(LushLookAndFeel::colourAccent);
        g.fillRoundedRectangle(topLabelBounds.toFloat(), 5);
        g.fillRoundedRectangle(bottomLabelBounds.toFloat(), 5);
    }

    void resized() override
    {
        updateBounds();
        
        auto boundsF = bounds.toFloat();
        
        crossFBsvg->setTransformToFit (boundsF, RectanglePlacement::stretchToFit);
        directFBsvg->setTransformToFit (boundsF, RectanglePlacement::stretchToFit);
        leftsvg->setTransformToFit (boundsF, RectanglePlacement::stretchToFit);
        rightsvg->setTransformToFit (boundsF, RectanglePlacement::stretchToFit);
        
        leftDelayLabel.setBounds(topLabelBounds);
        rightDelayLabel.setBounds(bottomLabelBounds);
    }
    
    void valueUpdated(stm::ParameterAttachment* attachment, float newValue) override {
        if (attachment == &crossFBAttachment) {
            crossFB = newValue;
        } else if (attachment == &directFBAttachment) {
            directFB = newValue;
        } else if (attachment == &delayAttachment) {
            delay = newValue;
        } else if (attachment == &offsetAttachment) {
            offset = newValue;
        } else if (attachment == &panAttachment) {
            pan = newValue;
        }
        
        updateComponents();
    }

private:
    std::unique_ptr<Drawable> crossFBsvg, directFBsvg, leftsvg, rightsvg;
    Label leftDelayLabel, rightDelayLabel;
    float whRatio = 400.0f / 160.0f;
    Rectangle<int> bounds, topLabelBounds, bottomLabelBounds;
    
    Colour crossFBColour = Colours::white, directFBColour = Colours::white, leftColour = Colours::white, rightColour = Colours::white;
    float crossFB, directFB, delay, offset, pan;
    stm::ParameterAttachment crossFBAttachment, directFBAttachment, delayAttachment, offsetAttachment, panAttachment;
    
    void initLabel(Label& label, String text) {
        addAndMakeVisible(label);
        //label.setText(text, dontSendNotification);
        label.setJustificationType(Justification::centred);
        label.setFont(Font(18.0f));
        //label.setColour(Label::backgroundColourId, Colours::black);
    }
    
    void updateBounds(){
        bounds = getLocalBounds();
        int height = bounds.getHeight();
        int width = bounds.getWidth();
        if (width / height > whRatio){
            //too much width
            int width = height * whRatio;
            int excessWidth = bounds.getWidth() - width;
            bounds.reduce(excessWidth/2, 0);
        } else {
            //too much height
            int height = width / whRatio;
            int excessHeight = bounds.getHeight() - height;
            bounds.reduce(0, excessHeight/2);
        }
        
        int labelHeight = 24;
        int labelWidth = 70;
        int reduceY = height * 0.18 - labelHeight/2;
        int reduceX = (width - labelWidth)/2;
        
        auto labelBounds = bounds.reduced(reduceX, reduceY);
        topLabelBounds = labelBounds.removeFromTop(labelHeight);
        bottomLabelBounds = labelBounds.removeFromBottom(labelHeight);
    }
    
    void updateComponents(){
        //------     UPDATE LABELS     -----------
        float leftDelay = delay;
        float rightDelay = delay;
        
        if ( offset < 0.5f ){
            float leftRatio = 1.0f - offset * 2.0f;
            leftDelay *= (1.0f + leftRatio);
        } else {
            float rightRatio = ( offset-0.5f ) * 2.0f;
            rightDelay *= (1.0f + rightRatio);
        }
        
        leftDelayLabel.setText(String::toDecimalStringWithSignificantFigures (leftDelay, 2) + "ms", dontSendNotification);
        rightDelayLabel.setText(String::toDecimalStringWithSignificantFigures (rightDelay, 2) + "ms", dontSendNotification);
        
        //-------     UPDATE SVGs     ------------
        auto balance = stm::Balancer::getLinearCentered(pan);
        crossFBsvg->replaceColour(crossFBColour, crossFBColour.withAlpha(crossFB));
        crossFBColour = crossFBColour.withAlpha(crossFB);
        directFBsvg->replaceColour(directFBColour, directFBColour.withAlpha(directFB));
        directFBColour = directFBColour.withAlpha(directFB);
        leftsvg->replaceColour(leftColour, rightColour.withAlpha(balance.left));
        leftColour = rightColour.withAlpha(balance.left);
        rightsvg->replaceColour(rightColour, rightColour.withAlpha(balance.right));
        rightColour = rightColour.withAlpha(balance.right);
        
        //-------------
        repaint();
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayVisualizer)
};
