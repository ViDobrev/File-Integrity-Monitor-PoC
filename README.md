## File Integiry Monitor
###### Proof of concept

A simple prototype for a file integrity monitor, written in C++.\
Uses SHA256 to track changes made to files.\
Last compiled with **OpenSSL v3.0.7**.\
Written in Visual Studio 2022.\
\
#### How to use:
**add path_to_file** to add the specified file to the monitor's list.\
**add folder path_to_folder** to add all files found inside the specified directory. (Non recursive)\
**remove path_to_file** to remove the specified file from the monitor's list.\
**remove folder path_to_folder** to remove all files found inside the specified directory. (Non recursive)\
**update path_to_file** to update the specified file's baseline hash.\
**update folder path_to_folder** to update the hashes of all files found inside the specified directory. (Non recursive)\
**start** to initiate the monitor.\
\
If user needs to execute one or more commands while the monitor is active, pressing **Enter** pauses it, so the input does not get interrupted.\
\
**exit/quit** to terminate program.