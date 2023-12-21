# CSC234_Project_Instructions
-Example for compiling and executing C files in the terminal: https://www.wikihow.com/Run-C-Program-in-Command-Prompt

-The main software is in Proj_Interpreter_V5.1.c but you will use main.c to call it. The rest of the .c files in the root of the folder (num_check.c, output_print.c, etc.) are sub modules that are not to be recompiled or turned into executables individually. Compiling and executing main.c and main.exe will recompile Proj_Interpreter_V5.1.c and the sub modules if changes were made:

    1A. If you DO NOT need to recompile Proj_Interpreter_V5.1.c or any other sub module then navigate to the "output" folder. Once there type into the terminal: ".\main.exe your_desired_filename_path.txt" without the quotes. This will execute "main.exe" with your desired file name. The ouputs will be printed to the terminal as well as written to output.txt.

    1B. If you DO need to recompile Proj_Interpreter_V5.1.c or any other sub module you will then need to type: "gcc .\main.c -o .\output\main.exe" into the terminal. Then navigate to the "output" folder and in the terminal type: ".\main.exe your_desired_filename_path.txt" without the qoutes to execute with your desired file name. The ouputs will be printed to the terminal as well as written to output.txt.

-All test executable files are in the folder "tests". There are test folders for every sub module as well as a "test_texts" folder which contains all .txt files used or referenced for testing:

    1. You must navigate to the folder of the module you want to test.

    2A. If you DO NOT need to recompile and create a new executable for a test file then navigate to the "output" folder. Once there type the ".\filename.exe" of the test file you wish to run in the terminal. The outputs will only be printed to the terminal.

    2B. If you DO need to recompile and create a new executable for a test file you must follow the same instructions given above:
        -You will need to type: "gcc .\your_desired_test_filename.c -o .'output\your_desired_test_filename.exe" into the terminal to compile and create an executable. Then navigate to the "output" folder and type: ".\your_desired_test_filename.exe" into the terminal to execute it. The outputs will only be printed to the terminal.


