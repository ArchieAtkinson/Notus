# COMET/RTE

This document is to detail the design of the Coffee Scales using the COMET/RTE (Concurrent Object Modeling and Architectural Design Method for Real-Time Embedded systems) method from Real-Time Software Design for Embedded Systems by Hassan Gomaa.

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
    " as coffeescale

    state 
    "
        << Output Device >>
        Display
    " as display   

    state 
    "
        << Input Device >>
        Load Cell
    " as loadcell  

    state 
    "
        << Input Device >>
        Button
    " as button 

    state 
    "
        << Timer >>
        Timer
    " as timer 

    state 
    "
        << Input/Output Device >>
        Battery Charger
    " as battery_charger 



    display --> coffeescale : 1 is part of
    loadcell --> coffeescale : 1 is part of
    button --> coffeescale : 3 is part of
    timer --> coffeescale : 1 is part of
    battery_charger --> coffeescale : 1 is part of

    classDef centerText text-align: center

    class coffeescale, display, loadcell, button, timer,battery_charger centerText
```

### System Context Model

```mermaid
stateDiagram-v2
    direction LR
    state 
    "
        << Embedded System >>
        Coffee Scale
    " as coffeescale

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

    user --> coffeescale : Interacts with

    user --> usb : Interacts with

    usb --> coffeescale : Interfaces

    classDef centerText text-align: center

    class coffeescale, user, usb centerText
```

### Software System Context Model 

```mermaid
stateDiagram-v2
    state 
    "
        << Embedded System >>
        Coffee Scale
    " as coffeescale

    state 
    "
        << External Output Device >>
        Display
    " as display   

    state 
    "
        << External Input Device >>
        Load Cell
    " as loadcell  


    state 
    "
        << External Input Device >>
        Button
    " as button 

    state 
    "
        << Internal Timer >>
        Timer
    " as timer 

    state 
    "
        << Input/Output Device >>
        Battery Charger
    " as battery_charger 

    coffeescale --> display : [1] Output to [1]
    coffeescale --> battery_charger : [1] Outputs to [1]

    timer --> coffeescale : [1] Signals to [1]

    battery_charger --> coffeescale : [1] Inputs to [1]
    
    loadcell --> coffeescale : [1] Inputs to [1]
    button --> coffeescale : [3] Inputs to [1]

    classDef centerText text-align: center

    class coffeescale, display, loadcell, button, timer,battery_charger centerText
```
