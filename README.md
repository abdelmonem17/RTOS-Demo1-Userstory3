# RTOS-Demo1-Userstory3
RTOS demo project

Use an RTOS project to define three tasks with the following specifications.
1.	Task A: Responsible for monitoring push button 1 state and sending data to LCD.
2.	Task B: Responsible for monitoring push button 2 state and sending data to LCD.
3.	Task C: Responsible for writing data on LCD.
Task C shall read a queue continuously and print the content of the data if found, The data shall contain two information:
1.	Any text with maximum size 16 character.
2.	Which button sent this text (Button ID). 
If button 1 is pressed then Task A shall write to the queue (Any text + Its ID), And if button 2 is pressed then Task B shall write to the queue (Any text + Its ID). If button ID belongs to button 1 then the corresponding text will be displayed on row 1 of the LCD, And If button ID belongs to button 2 then the corresponding text will be displayed on row 2 of the LCD. Any text displayed on the LCD shall be displayed for 500ms and cleared.
