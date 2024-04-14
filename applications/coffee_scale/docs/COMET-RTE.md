# COMET/RTE

This document is to detail the design of the Coffee Scales using the COMET/RTE (Concurrent Object Modeling and Architectural Design Method for Real-Time Embedded systems) method from Real-Time Software Design for Embedded Systems by Hassan Gomaa.

## Content
- [COMET/RTE](#cometrte)
  - [Content](#content)
  - [Problem Description](#problem-description)
  - [Structural Modeling](#structural-modeling)
    - [Conceptual Structural Model of Problem Domain](#conceptual-structural-model-of-problem-domain)
    - [System Context Model](#system-context-model)
    - [Software System Context Model](#software-system-context-model)
    - [Hardware/Software Interface](#hardwaresoftware-interface)
    - [Thoughts](#thoughts)


## Problem Description

The Coffee Scales is a simple, easy to use set of scales aimed at pour over coffee making. Key features are:
- Fast Startup
- Waterproof
- Tactile Buttons
- Open Source Design
- Rechargeable

The scales will have three physical tactile buttons. One to turn the device on and off, one to tare the scale and one to control the stopwatch. A single press of the stopwatch button will start the timer, another press will pause it. A double press will pause the timer and reset it to zero. It will also have a load cell capable of measuring up a mass up to 2kg within 0.1g. The scales will have a display that will show the stopwatch time, weight on the scale and battery life. The display should also be capable of displaying errors or warnings such as if the mass on the scale is too large. The scale will be battery powered and recharged over USB C. 

## Structural Modeling

### Conceptual Structural Model of Problem Domain


```mermaid
stateDiagram-v2
    
    state 
    "
        << Embedded System >>
        Coffee Scale
    " as coffee_scale

    state 
    "
        << Output Device >>
        Display
    " as display   

    state 
    "
        << Input Device >>
        Load Cell
    " as load_cell  

    state 
    "
        << Input Device >>
        Button
    " as button 

    state 
    "
        << Input/Output Device >>
        Battery Charger
    " as battery_charger 

    display --> coffee_scale : 1 is part of
    load_cell --> coffee_scale : 1 is part of
    button --> coffee_scale : 3 is part of
    battery_charger --> coffee_scale : 1 is part of

    classDef centerText text-align: center

    class coffee_scale, display, load_cell, button, battery_charger centerText
```

### System Context Model

```mermaid
stateDiagram-v2
    direction LR
    state 
    "
        << Embedded System >>
        Coffee Scale
    " as coffee_scale

    state 
    "
        << User >>
        User
    " as user   

    state 
    "
        << Input >>
        USB C
    " as usb   

    user --> coffee_scale : Interacts with

    user --> usb : Interacts with

    usb --> coffee_scale : Interfaces

    classDef centerText text-align: center

    class coffee_scale, user, usb centerText
```

### Software System Context Model 

```mermaid
stateDiagram-v2
    state 
    "
        << Embedded System >>
        Coffee Scale
    " as coffee_scale

    state 
    "
        << External Output Device >>
        Display
    " as display   

    state 
    "
        << External Input Device >>
        Load Cell
    " as load_cell  


    state 
    "
        << External Input Device >>
        On/Off Button
    " as on_off_button 

    state 
    "
        << External Input Device >>
        Tare Button
    " as tare_button 


    state 
    "
        << External Input Device >>
        Stopwatch Button
    " as stopwatch_button 

    state 
    "
        << Input/Output Device >>
        Battery Charger
    " as battery_charger 

    coffee_scale --> display : Output to
    coffee_scale --> battery_charger : Outputs to

    battery_charger --> coffee_scale : Inputs to
    
    load_cell --> coffee_scale : Inputs to
    on_off_button --> coffee_scale : Inputs to
    tare_button --> coffee_scale : Inputs to
    stopwatch_button --> coffee_scale : Inputs to

    classDef centerText text-align: center

    class coffee_scale, display, load_cell, on_off_button, tare_button, stopwatch_button, battery_charger centerText
```

### Hardware/Software Interface

|Device name|Device type|Device function|Inputs from device|Outputs to device|
|---|---|--|---|---|
|Battery Charger|Input/Output|Chargers Battery|Charging Information|Power|
|Display|Output|Display information to user||Stopwatch time, Weight, Errors|
|Load Cell|Input|Measure Weight|Weight||
|On/Off Button|Input|User Control|Turn On, Turn off||
|Tare Button|Input|User Control|Tare Scale||
|Stopwatch Button|Input|User Control|Start, Stop, Rest Stopwatch||

### Thoughts
 
Real-Time Software Design for Embedded Systems seems aimed at more complex system than what I am working on here, so some concepts do not carry across. Part of that is understanding the level of detail the structural modeling should include. The examples are typically high level. This has lead to the decision to not included outlining the use of the internal ADC for measuring battery voltage for estimating state of charge, the internal timer for the stopwatch or the interaction between the battery and the battery charger. We shall see if the later stages of the methodology will have a place for them.  