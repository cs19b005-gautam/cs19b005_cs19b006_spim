# cs19b005_cs19b006_spim
spim simulator of CS19B005,CS19B006


We team of CS19B005 and CS19B006 had done our MIPS simulator in this following way


we read the whole file(entered first after running)
we store each line as string push_back to a vector vec
(vec[i] implies line (i+1) in the file selected)

whenever we saw a label in .text we will store this in another vector int_text_label_array which contains 
all labels in  .text segment of the MIPS assembly code


(NOTE: whenever a label is given in the code make sure no character is present after 
the label for this simulator)
Ex:

	label:
		add $t3,$t2,$t1			is the correct method


label:		add $t3,$t2,$t1			is not supported for this simulator


now as all lines will be read by using file_line_read function
now if it belongs to .data all values will get stored to memory by file_data_line_read function based on the string line it took
if it's .text string line will be sent to file_line_text_read function 


In file_line_text_read every instruction is made into parts of opcode and number of operands present
now these will be sent to function perform_string_line_operation
the register fetch and arithmetic operation based on opcode and writeback to registers will be done


NOTE : comments working are not tested in this simulator
