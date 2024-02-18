# Error Handling

## Error Types

### Exceptions

Exceptions are rare failures of the code at runtime. The Coffee Scale is a relatively simple device so it only has a few exceptional errors, which are listed below: 
- Initializations Failure
- Sensor Failure
- Battery/Charger Issue
- Over/Under Temperate
  
When these errors happen a `[[Insert custom exception class]` should be thrown for them to be handled. 

Due to the simple nature of the device, most exceptions will lead to restarting the device as there is nothing the program or user can do to solve the issue. The flow is detailed below:

```mermaid
flowchart TB
    error(["Error!"]) --> display_init{{"Has the Display been initialized?"}}

    display_init --> |Yes| display_error("Display error to user")
    display_error --> wait("Wait for 10 seconds")

    wait --> turn_off("Turn off device")o

    display_init --> |No| turn_off
```

Question:     
Is it worth including a red LED for signalling errors if the display driver doesn't work? Or maybe USB serial output? 

### User Errors

There is only one user error possible in the system:
- Scale overload due to too much weight

This error will be reported by displaying a warning on the display is instead of the measured weight.
