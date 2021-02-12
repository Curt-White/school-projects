# arm-mu-compansion

sample wave from: https://www2.cs.uic.edu/~i101/SoundFiles/

parsing wav file technique: http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/

## Tests

To run the tests execute:

	make build

There should be two executables. First run the first compress and then run expand. After this, any of the other files can be executed and they will be test correctly. If any values print then this means there is an error in the output and the two mismatched values will be shown.

## Project Layout

**/expand-code** contains all of the code which was used to expand the compressed audio 

	expand_orig.c is the original code, the first attempt at decompressing the audio files 

	expand_pipe_dec.c is a version of the pipeline with decremeneting loop.

	expand_pipe_lut_reordered.c this is a reoredered version of the original LUT code. This original version has been deleted due to an error in the calculation process.

	expand_shift.c is an expansion process which is intended to be a quick alternative to the if and switch options.

	expand_switch.c is also one of the original options tried to compare with expand_shift.c and expand_orig.c.

	final.c is the final sw pipelining code that was created.

	final.S has some assembly modifcations to the inner loop of the final.c code. This is starting at line 56.

	new_instr.c is an example of calling the new instruction in c

	new_instr.S is an example of calling the new instruction in assembly

mu_compress.c is the code used to make a file of compressed samples from the sample.wav file. It outputs a file called compressed.txt.

mu_expand.c is the code used to make a list of expanded code words which can be checked against. Many of the outputs for this file were checked manually for correctness. This file outputs a file called expanded.txt

test.h is a testing harness which was used in our code. All of the files can pass in a function pointer to an expand function which takes an input list and an output list and the number of items to process and it expands all items in the input list and places the values in the output list. These values are then checked against expanded.txt and the user is notified if there are any inconsistencies.

wav.h is a header library for opening and closing wav files. There is an open and a close function were the information can be read from sample.wav. The output function can be used to write the  data to a file to ensure it still sounds correct.
