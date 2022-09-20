#  dsp-ctrl

A GUI for controlling a hardware DSP device.

## Building

This project uses juce 7 with C++14.
Compile using either MSVC, XCode or Makefile.
Modifying the project requires projucer.

## References

- This project takes inspiration and partial code from the [Freequalizer Project](https://github.com/ffAudio/Frequalizer).
- IIR formulas are taken from [robert bristow-johnson's Audio EQ Cookbook](https://webaudio.github.io/Audio-EQ-Cookbook/Audio-EQ-Cookbook.txt).
- A numerically stable mathematical solution for determining IIR magnitude stems from [robert bristow-johnson's answer on the DSP Stackexchange](https://dsp.stackexchange.com/a/16911/56072). 
- This uses the [juce serialport](https://github.com/cpr2323/juce_serialport) module as a dependency. 
