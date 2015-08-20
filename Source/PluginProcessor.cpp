/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CamomileAudioProcessor::CamomileAudioProcessor()
{
    try
    {
        m_pd = Master::createInstance();
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << "\n";
    }
    if(m_pd)
    {
        try
        {
            m_patch = m_pd->openPatch("Test2.pd", "/Users/Pierre/Desktop/");
        }
        catch(std::exception& e)
        {
            std::cout << e.what() << "\n";
        }
    }
    /*
    AudioProcessorParameter parameter;
    addParameter(parameter);
     */
}

CamomileAudioProcessor::~CamomileAudioProcessor()
{
    lock_guard<mutex> guard(m_mutex);
    m_listeners.clear();
}

int CamomileAudioProcessor::getNumParameters()
{
    return 64;
}

const String CamomileAudioProcessor::getParameterName(int index)
{
    if(m_pd)
    {
        sPatch patch = m_patch.lock();
        if(patch)
        {
            int count = 0;
            vector<sGui> objects(patch->getGuis());
            for(auto it : objects)
            {
                if(it->hasPresetName())
                {
                    if(count++ == index)
                    {
                        return String(it->getPresetName());
                    }
                }
            }
        }
    }
    return String();
}

float CamomileAudioProcessor::getParameter (int index)
{
    return 0.0f;
}

void CamomileAudioProcessor::setParameter (int index, float newValue)
{
}

const String CamomileAudioProcessor::getParameterText (int index)
{
    return String();
}

bool CamomileAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CamomileAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

void CamomileAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if(m_pd)
    {
        try
        {
            m_pd->prepareDsp(getNumInputChannels(), getNumOutputChannels(), sampleRate, samplesPerBlock);
        }
        catch(std::exception& e)
        {
            std::cout << e.what() << "\n";
        }
    }
}

void CamomileAudioProcessor::releaseResources()
{
    if(m_pd)
    {
        m_pd->releaseDsp();
    }
}

void CamomileAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    for(int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
    if(m_pd)
    {
        m_pd->processDsp(buffer.getNumSamples(),
                         getNumInputChannels(), buffer.getArrayOfReadPointers(),
                         getNumOutputChannels(), buffer.getArrayOfWritePointers());
    }
}

AudioProcessorEditor* CamomileAudioProcessor::createEditor()
{
    return new CamomileInterface(*this);
}

void CamomileAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    if(m_pd)
    {
        sPatch patch = m_patch.lock();
        if(patch)
        {
            XmlElement xml("CamomileSettings");
            xml.setAttribute("name", patch->getName());
            xml.setAttribute("path", patch->getPath());
            copyXmlToBinary(xml, destData);
        }
    }
}

void CamomileAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if(xml != nullptr)
    {
        if(xml->hasTagName("CamomileSettings"))
        {
            String name = xml->getStringAttribute("name");
            String path = xml->getStringAttribute("path");
            
            File file(path + "/" + name);
            loadPatch(file);
        }
    }
}

void CamomileAudioProcessor::loadPatch(const juce::File& file)
{
    if(m_pd)
    {
        suspendProcessing(true);
        if(isSuspended())
        {
            {
                lock_guard<mutex> guard(m_mutex);
                sPatch patch = m_patch.lock();
                if(patch)
                {
                    m_pd->closePatch(patch);
                }
                if(file.exists() && file.getFileExtension() == String(".pd"))
                {
                    try
                    {
                        m_patch = m_pd->openPatch(file.getFileName().toStdString(), (file.getParentDirectory()).getFullPathName().toStdString());
                    }
                    catch(std::exception& e)
                    {
                        std::cout << e.what() << "\n";
                    }
                }
            }
            
            vector<Listener*> listeners = getListeners();
            for(auto it : listeners)
            {
                it->patchChanged();
            }
            updateHostDisplay();
        }
        
        suspendProcessing(false);
    }
    
}

void CamomileAudioProcessor::addListener(Listener* listener)
{
    if(listener)
    {
        lock_guard<mutex> guard(m_mutex);
        m_listeners.insert(listener);
    }
}

void CamomileAudioProcessor::removeListener(Listener* listener)
{
    if(listener)
    {
        lock_guard<mutex> guard(m_mutex);
        m_listeners.erase(listener);
    }
}

vector<CamomileAudioProcessor::Listener*> CamomileAudioProcessor::getListeners() const noexcept
{
    lock_guard<mutex> guard(m_mutex);
    return vector<Listener*>(m_listeners.begin(), m_listeners.end());
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CamomileAudioProcessor();
}


