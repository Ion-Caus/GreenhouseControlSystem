# IoT part of semester project

to be able to build the project you need to remove `GreenhouseControlSystem/FreeRTOS/Example/main.c` file.

## testing
testing in this project is made using google test and mocking.
to test something there is test solution for visual studio.

To test some new .c file you need to add the file to the project _right click on solution -> add -> existing file -> navigate to the .c file_.

tese test will work also for FreeRTOS but you have to **rename ATMEGA_FreeRTOS.h to FreeRTOS.h** [^1]

[^1]: showcase of testing was made just with temperature when the temperature tasks was commented out 
the reason is that it was just example test so not all mocked up methods were created and also that
it is impossible to test method that never returns.
