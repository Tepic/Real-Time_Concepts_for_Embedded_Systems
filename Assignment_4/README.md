** c. and .h file Explanations **
1. workerTask.h 
   - Stores all information about the tasks
2. gll.h
   - Generic Doubly Linked List   
3. bool_t.h
   - Typedefs for boolean data type
4. acquiredResource.h
   - Used by workerTask.h to store information required to implement Priority Inheritance Protocol
5. semaphore.h
   - Wrapper for FreeRTOS' semaphore
   - Enables both Priority Inheritance and Immediate Priority Ceiling Protocol   
6. print.h
   - Used to write output to the console
7. debug.h
   - Enable/Disable Debug Mode
   - With Debug Enabled output will be logged onto the console
8. test.h
   - stores all the tests
