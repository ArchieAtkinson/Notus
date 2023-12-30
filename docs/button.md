# Button
Goals:

- Debounce
- Input Types
  - Single/Shot Press
  - Double Press
  - Triple Press
  - Long Press
  - Longer Press? 
  - On Press
  - On Release?
- System Work Queue, Custom Thread, in ISR? 


The goal of the Button class is to abstract GPIO interactions and allow the user to react to any type of button press.


Different types of button press are:

## On Press
The On Press callback reacts to a button press as soon it is first detected, this is either after the `debounce_time` or if instantly if debounce is disabled. 


