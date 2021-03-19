//	1. label identification 
//	2. label pointer array
//bubble sort MIPS assmenbly code
//blt	$t0,$t1,label

//1.instruction Fetch
//2.Instruction Decode/	Register Fetch
//3.Execute
//4.Memory
//5.Write Back

#include <bits/stdc++.h>

using namespace std;
std::vector<string> vec;//file lines one by one
//std::set<string> commands = {,"add","sub","lw","j",,"mul","sw"};

int program_counter;	//	NEED TO BE INTIALISED IN MAIN MOST PROBABLY
//int 4X 10240 , char -> 40960 //40KB
int memory[10240] = { 0 }; //1 element->1 byte, 1KB->1024 bytes, 4KB->4096,this memory has 40KB/  //character 4 bits - 1 // 2) string 
int memory_ptr = 0; //memory_ptr
int reg[32] = { 0 };
int low_high[2] = { 0 };
int data_enable = 1;
struct int_data_label {//label:	.word	23,34,54
	string label;
	std::vector<int> values;
	int memory_index;
};
std::vector<int_data_label> int_data_label_array;
//int int_regs_reserved[5] = { 0 };	0->PC,1->EPC,2->BadVAddr,3->Status				/*Status usually have a value I guess*/
//float fp_register_single_precision[32] = { 0 };
//double fp_register_double_precision[32] = { 0 };
//int fp_regs_reserved[4] = { 0 };
//int enable_text_segment = 0;
std::vector<string> registers1 = { "$0","$1","$2","$3","$4","$5","$6","$7","$8","$9","$10","$11","$12","$13","$14","$15","$16","$17","$18","$19","$20","$21","$22","$23","$24","$25","$26","$27","$28","$29","$30","$31" };
std::vector<string> registers2 = { "$r0","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$s8","$ra" };

void data_to_memory(int_data_label d) {
	d.memory_index = memory_ptr;
	int i = 0;
	for(;memory_ptr<d.values.size();memory_ptr++){
		memory[memory_ptr] = d.values[i];
		i++;
	}
	int_data_label_array.push_back(d);
}

struct text_label {
	string label;
	int line_index;
};

std::vector<text_label> text_label_array;

void jump(string str) {
	for (int i = 0; i < text_label_array.size(); i++) {
		if (text_label_array[i].label == str) {
			program_counter = (text_label_array[i].line_index);
			return;
		}
	}
}

bool int_str(string str) {
	bool only_int_char_exist = true;
	for (int i = 0; i < str.length(); i++) {
		if (str[i] < '0' || str[i]>'9') {
			only_int_char_exist = false;
		}
	}
	return only_int_char_exist;
}

int register_index(string str) {
	if (str == "$zero")
		return 0;
	int index = -1;
	for (int i = 0; i < 32; i++) {
		if (str == registers1[i] || str == registers2[i]) {
			index = i;;
			break;
		}
	}

	return index;
}

bool contains_register(string str) {
	for (int i = 0; i < str.length(); i++) {
		if (str[i] == '$') {
			return true;
		}
	}
	return false;
}

int get_data_memory_index(string str) {
	for (int i = 0; i<int_data_label_array.size(); i++) {
		if (int_data_label_array[i].label == str) {
			return int_data_label_array[i].memory_index;
		}
	}
}



int load_store_mem(string str) {//this function is for getting adresses to registers||we can get adresses by 0($t0) , ($t0) , array, array($t0)
	if (!contains_register(str)) {//lablel
		return get_data_memory_index(str);
	}
	else {
		string offset = "";
		string register_string = "";
		if (str[0] == '(' && str[str.length() - 1] == ')') { //offset = ""  register_string = "$t0"		($t0)
			register_string = str.substr(1, str.length() - 1 - 1);
			int num = reg[register_index(register_string)];
			num = num / 4;
			return num;
		}
		else{//0($t0), array($t0)
			int i;
			for (i = 0; i < str.length(); i++) {
				if (str[i] == '(') {
					break;
				}
			}
			offset = str.substr(0, i);
			register_string = str.substr(i + 1,str.length() - i - 2);
			int num = reg[register_index(register_string)];
			num = num / 4;
			if (int_str(offset)) {//32($t0)
				int o = stoi(offset);
				o = o / 4;
				return (num + o);
			}
			else {
				int o = get_data_memory_index(offset);
				return (o + num);
			}
		}

	}
}



void perform_string_data_line_operation(string opcode,string operand1, string operand2){//making .data into memort
	int_data_label temp_int_data_label;
	temp_int_data_label.label = opcode.substr(0, opcode.length() - 1);
	if (int_str(operand2)) {
		temp_int_data_label.values.push_back(stoi(operand2));
	}
	else {
		int num = 0;
		for (int i = 0; i < operand2.length(); i++) {
			if (operand2[i] == ',') {
				temp_int_data_label.values.push_back(num);
				num = 0;
			}
			else if(operand2[i]=='0' && operand2[i+1]==',' && operand2[i-1]==',') {
				temp_int_data_label.values.push_back(0);
			}
			else {
				if (i == operand2.length() - 1) {
					num = (10*num) + (operand2[i]-'0');
					temp_int_data_label.values.push_back(num);
				}
				else {
					num = (10*num) + (operand2[i] - '0');
				}
			}
		}
	}
	//data_word.push_back(l);
}

void perform_string_line_operation(string opcode) {//	MAYBE BEST IF WE PASS A PAPRAMETER AS A STRING PARAMETER?
	if (opcode == "syscall") {
		//evaluate$v0;
		//if $v0<=4	print
		//else $v0>=5	take input into $v0
	}
}

void perform_string_line_operation(string opcode, string operand1) {//	MAYBE BEST IF WE RETURN A STRING POINTER F
	//if	opcode	register		=>perform arithematic operation
	//		mflo, mfhi
	//if	j	label				=>jump to pointer 
	if (opcode == "mflo") {
		reg[register_index(operand1)] = low_high[0];
	}
	else if (opcode == "mfhi") {
		reg[register_index(operand1)] = low_high[1];
	}
	else if (opcode == "j") {
		jump(operand1);
	}
}

void perform_string_line_operation(string opcode, int num1) {
	//invalid
}

void perform_string_line_operation(string opcode, string operand1, string operand2) {//instruction decode
	//load,store,div,mul.move
	if (opcode == "div") {
		low_high[0] = reg[register_index(operand1)] / reg[register_index(operand2)];
		low_high[1] = reg[register_index(operand1)] % reg[register_index(operand2)];
	}
	else if (opcode == "mul") {
	
	}
	else if (opcode == "move") {
		reg[register_index(operand1)] = reg[register_index(operand2)];
	}
	else if (opcode == "lw") {
		reg[register_index(operand1)] = memory[load_store_mem(operand2)];
	}
	else if (opcode == "la") {
		reg[register_index(operand1)] = load_store_mem(operand2);
	}
	else if (opcode == "sw") {
		memory[load_store_mem(operand2)] = reg[register_index(operand1)];
	}
}

void perform_string_line_operation(string opcode, int num1, string operand2) {
	//invalid
}

void perform_string_line_operation(string opcode, string operand1, int num2) {
	if (opcode == "li") {
		reg[register_index(operand1)] = num2;
	}
}

void perform_string_line_operation(string opcode, int num1, int num2) {
	//invalid
}

void perform_string_line_operation(string opcode, string operand1, string operand2, string operand3) {
	if (opcode == "add") {
		int result;
		result = reg[register_index(operand2)] + reg[register_index(operand3)];//execute//num2+get_register_index(operand3);
		reg[register_index(operand1)] = result;
	}
	else if (opcode == "sub") {
		int result;
		result = reg[register_index(operand2)] - reg[register_index(operand3)];//execute//num2+get_register_index(operand3);
		reg[register_index(operand1)] = result;
	}
	else if (opcode == "beq") {
		if (reg[register_index(operand1)] == reg[register_index(operand2)]) {
			jump(operand3);
		}
	}
	else if (opcode == "bne") {
		if (reg[register_index(operand1)] != reg[register_index(operand2)]) {
			jump(operand3);
		}
	}
	else if (opcode == "blt") {
		if (reg[register_index(operand1)] < reg[register_index(operand2)]) {
			jump(operand3);
		}
	}
	else if (opcode == "bgt") {
		if (reg[register_index(operand1)] > reg[register_index(operand2)]) {
			jump(operand3);
		}
	}
	else if (opcode == "bge") {
		if (reg[register_index(operand1)] >= reg[register_index(operand2)]) {
			jump(operand3);
		}
	}
	else if (opcode == "ble") {
		if (reg[register_index(operand1)] <= reg[register_index(operand2)]) {
			jump(operand3);
		}
	}
	else if (opcode == "slt") {
		reg[register_index(operand1)] = reg[register_index(operand2)] < reg[register_index(operand3)];
	}
	else if (opcode == "sgt") {
		reg[register_index(operand1)] = reg[register_index(operand2)] > reg[register_index(operand3)];
	}
}

void perform_string_line_operation(string opcode, string operand1, string operand2, int num3) {
	if (opcode == "add") {
		reg[register_index(operand1)] = reg[register_index(operand2)] + num3;
	}
	else if (opcode == "sub") {
		reg[register_index(operand1)] = reg[register_index(operand2)] - num3;
	}
	else if (opcode == "mul") {

	}
}

void perform_string_line_operation(string opcode, string operand1, int num2, string operand3) {
	if (opcode == "add") {
		int result;
		result = num2 + reg[register_index(operand3)];//execute//num2+get_register_index(operand3);
		reg[register_index(operand1)] = result;//set_result(operand1)
	}
	else if (opcode == "sub") {
		int result;
		result = num2 - reg[register_index(operand3)];//execute//num2+get_register_index(operand3);
		reg[register_index(operand1)] = result;//set_result(operand1)
	}
	else if (opcode == "mul") {
		int result;
		result = num2 * reg[register_index(operand3)];//execute//num2+get_register_index(operand3);
		reg[register_index(operand1)] = result;//set_result(operand1)
	}
}

void perform_string_line_operation(string opcode, string operand1, int num2 , int num3) {
	if (opcode == "add") {
		int result;
		result = num2+num3;//execution
		reg[register_index(operand1)] = result ;//write back
	}
	else if (opcode == "sub") {
		int result;
		result = num2 - num3;//execution
		reg[register_index(operand1)] = result;//write back
	}
	else if (opcode == "mul") {
		int result;
		result = num2 * num3;//execution
		reg[register_index(operand1)] = result;//write back
	}
}

void perform_string_line_operation(string opcode, int num1, string operand2, string operand3) {
	//invalid
}

void perform_string_line_operation(string opcode, int num1, string operand2, int num3) {
	//invalid
}

void perform_string_line_operation(string opcode, int num1, int num2, string operand3) {
	//invalid
}

void perform_string_line_operation(string opcode, int num1, int num2, int num3) {
	//invalid
}


void file_line_text_read(string str) {//opcode->li,add,sw,lw,sub,mul   operands(1/2/3)->destination registers,1/2source registers,   .dat
	//instruction fetch
	if (str == "" || str == "\t" || str == "\t\t" || str == "\t\t\t") {
		return;
	}

	//how to implement an array without different data-types in C++  ?

	//.text		enable Instruction read
	string opcode = "";//can be a opcode or a label defintion 
	string operand1 = "";// can be a register or a number also  or a label callee
	string operand2 = "";
	string operand3 = "";
	//strings without instructions

	int i, num_of_operands = 0;
	int start_opcode_index = -1;
	for (i = 0; i < str.length(); i++) {	//make into 3 or 4 parts		"	li $v0"
		if (str[i] == ' ' || str[i] == '\t') {
			if (start_opcode_index == -1) {
				continue;
			}
			else {
				break;
			}
		}
		else if (str[i] >= 'a' && str[i] <= 'z' && start_opcode_index == -1) {
			start_opcode_index = i;
		}
	}
	opcode = str.substr(start_opcode_index, i - start_opcode_index);


	//operand1
	int start_operand_index = -1;
	for (; i < str.length(); i++) {
		if (str[i] == ' ' || str[i] == '\t' || str[i] == ',') {
			if (start_operand_index != -1) {
				break;
			}
			else {
				continue;
			}
		}
		else if (str[i] == '$') {
			start_operand_index = i;
			break;
		}//find out where next space is
		else if (str[i] <= '9' && str[i] >= '0') {
			start_operand_index = i;
			break;
		}
		else if ((opcode == "j" || opcode == "jal") && ((str[i] <= 'z' && str[i] >= 'a') || (str[i] <= 'Z' && str[i] >= 'A'))) {	// for label
			start_operand_index = i;
			break;
		}
	}
	for (; i < str.length(); i++) {
		if (str[i] == ' ' || str[i] == '\t' || str[i] == ',') {
			break;
		}
	}
	if (start_operand_index != -1) {
		operand1 = str.substr(start_operand_index, i - start_operand_index);
		num_of_operands++;
	}
	int num1;
	if (operand1 != "" && int_str(operand1)) {
		num1 = stoi(operand1);
	}


	//operand2
	start_operand_index = -1;
	for (; i < str.length(); i++) {
		if (str[i] == ' ' || str[i] == '\t' || str[i] == ',') {
			if (start_operand_index != -1) {
				break;
			}
			else {
				continue;
			}
		}
		else {
			start_operand_index = i;
			break;
		}//find out where next space is
		
	}
	for (; i < str.length(); i++) {
		if (str[i] == ' ' || str[i] == '\t' || str[i] == ',') {
			break;
		}
	}
	if (start_operand_index != -1) {
		operand2 = str.substr(start_operand_index, i - start_operand_index);
		num_of_operands++;
	}
	int num2;
	if (operand2 != "" && int_str(operand2)) {
		num2 = stoi(operand2);
	}
	

	//operand3
	start_operand_index = -1;
	for (; i < str.length(); i++) {
		if (str[i] == ' ' || str[i] == '\t' || str[i] == ',') {
			if (start_operand_index != -1) {
				break;
			}
			else {
				continue;
			}
		}
		else {
			start_operand_index = i;
			break;
		}//find out where next space is
		
	}
	for (; i < str.length(); i++) {
		if (str[i] == ' ' || str[i] == '\t' || str[i] == ',') {
			break;
		}
	}
	if (start_operand_index != -1) {
		operand3 = str.substr(start_operand_index, i - start_operand_index);
		num_of_operands++;
	}
	int num3;
	if (operand3 != "" && int_str(operand3)) {
		num3 = stoi(operand3);
	}
	if (i != str.length()) {
		operand3 = str.substr(start_operand_index, i - start_operand_index);
		num_of_operands++;
	}
	
	if (opcode == "syscall" && reg[register_index("$v0")] == 10) {
		program_counter = vec.size();
		return;
	}

	if (operand1 == "") {
		perform_string_line_operation(opcode);
	}
	else {
		if (operand3 == "") {
			if (operand2 == "") {
				if (int_str(operand1)) {
					perform_string_line_operation(opcode, num1);
				}
				else {
					perform_string_line_operation(opcode, operand1);
				}
			}
			else {
				if (int_str(operand1)) {
					if (int_str(operand2)) {
						perform_string_line_operation(opcode, num1, num2);
					}
					else {
						perform_string_line_operation(opcode, num1, operand2);
					}
				}
				else {
					if (int_str(operand2)) {
						perform_string_line_operation(opcode, operand1, num2);
					}
					else {
						perform_string_line_operation(opcode, operand1, operand2);
					}
				}
			}
		}
		else {
			if (int_str(operand3)) {
				if (int_str(operand2)) {
					if (int_str(operand1)) {
						perform_string_line_operation(opcode, num1, num2, num3);
					}
					else {
						perform_string_line_operation(opcode, operand1, num2, num3);
					}
				}
				else {
					if (int_str(operand1)) {
						perform_string_line_operation(opcode, num1, operand2, num3);
					}
					else {
						perform_string_line_operation(opcode, operand1, operand2, num3);
					}
				}
			}
			else {
				if (int_str(operand2)) {
					if (int_str(operand1)) {
						perform_string_line_operation(opcode, num1, num2, operand3);
					}
					else {
						perform_string_line_operation(opcode, operand1, num2 , operand3);
					}
				}
				else {
					if (int_str(operand1)) {
						perform_string_line_operation(opcode, num1 , operand2, operand3);
					}
					else {
						perform_string_line_operation(opcode, operand1, operand2, operand3);
					}
				}
			}
		}
	}
	
}

void file_line_data_read(string str) {
	if (str == "" || str == "\t" || str == "\t\t" || str == "\t\t\t") {
		return ;
	}
	string label = "", type = "", values = "";
	int start_index = -1;
	int i = 0;
	for (i = 0; i < str.length(); i++) {
		if (str[i] == ' ' || str[i] == '\t') {
			if (start_index == -1) {
				continue;
			}
			else {
				break;
			}
		}
		else {
			if (start_index == -1) {
				start_index = i;
			}
		}
	}
	if (i != 0 && str.substr(start_index, i - start_index) == ".data") {
		return ;
	}
	else if (i != 0) {
		label = str.substr(start_index, i - start_index - 1);
	}
	start_index = -1;
	for (; i < str.length(); i++) {
		if (str[i] == ' ' || str[i] == '\t') {
			if (start_index == -1) {
				continue;
			}
			else {
				break;
			}
		}
		else {
			if (start_index == -1) {
				start_index = i;
			}
		}
	}
	if (start_index >= 0) {
		type = str.substr(start_index, i - start_index);
	}
	start_index = -1;
	for (; i < str.length(); i++) {
		if (str[i] == ' ' || str[i] == '\t') {
			if (start_index == -1) {
				continue;
			}
			else {
				break;
			}
		}
		else {
			if (start_index == -1) {
				start_index = i;
			}
		}
	}
	if (start_index >= 0) {
		values = str.substr(start_index, str.length() - start_index);
	}
	int val;
	int num = 0;
	std::vector<int> arr;
	for (int i = 0; i < values.length(); i++) {
		if (values[i] == ',') {
			arr.push_back(num);
			num = 0;
		}
		else if (values[i] <= '9' && values[i] >= '0') {
			num = 10 * num + (values[i] - '0');
			if (i == values.length() - 1) {
				arr.push_back(num);
			}
		}
	}
	int_data_label temp;
	temp.label = label;
	temp.values = arr;
	temp.memory_index = -1;
	data_to_memory(temp);
}


void file_line_read(string str) {
	if (str == "" || str == "\t" || str == "\t\t" || str == "\t\t\t") {
		return;
	}
	string s;
	int i,index = -1;
	for (i = 0; i < str.length(); i++) {
		if (str[i] == ' '||str[i]=='\t') {
			if (index==-1) {
				continue;
			}
			else {
				break;
			}
		}
		else {
			if (index == -1) {
				index = i;
			}
		}
	}
	s = str.substr(index, i - index);
	

	if (s == ".data") {
		data_enable = 1;
	}
	else if (s == ".text") {
		data_enable = 0;

	}

	if (data_enable) {
		file_line_data_read(str);
	}
	else {
		file_line_text_read(str);
	}
}


int main() {
	string fileName;
	cout << "Please provide your file path" << endl << "(Please mention the file extension along with the file name while providing the file path) : " << endl ;


	getline(cin, fileName);
	fstream file;
	file.open(fileName, ios::in | ios::out );
	if (!file.is_open())
		cout << "ERROR OCCURRED WHILE OPENING THE FILE." << endl << "PLEASE PROVIDE A VALID FILE PATH.";
	else {
		cout << endl << "FILE OPENED SUCCESSFULLY" << endl ;
		string line;
		cout << "############################################################################################" << endl;
		cout << "\t\t\t FILE CODE" << endl;
		cout << "############################################################################################" << endl;
		while (file.good()) {
			getline(file, line);
			cout << line << endl;
			vec.push_back(line);///Add label vector which contains string pointer
			if (line.length()!=0 && line[line.length() - 1] == ':') {//Disadv : if any label in .text those have to be in a line separately
				//cout << &(vec[vec.size() - 1]) << endl;
				text_label temp_label;
				temp_label.label = line.substr(0, line.length() - 1);
				temp_label.line_index = vec.size()-1;
				text_label_array.push_back(temp_label);
				//pushing label into labels vector so whenever label is seen 'j label' program counter jumps to label
			}
		}

		program_counter = 0;
		int n;
		for (; program_counter<vec.size(); program_counter++) {
			n = program_counter;
			file_line_read(vec[program_counter]);
		}
		program_counter = n;
		cout << "#####################################################################################" << endl;
		cout << "\t\t\t\tREGISTERS" << endl;
		cout << "#####################################################################################" << endl;

		for (int i = 0; i < 16; i++) {
			cout << "R" << i;
			cout << " ";
			if (i <= 9) {
				cout << " ";
			}
			cout << "[" << registers2[i] << "] = ";
			cout << reg[i] << "\t\t\t";
			cout << "R" << i+16;
			cout << " ";
			cout << "[" << registers2[i+16] << "] = ";
			cout << reg[i+16] << endl;
		}

		cout << "#####################################################################################" << endl;
		cout << "\t\t\t\tMEMORY" << endl;
		cout << "#####################################################################################" << endl;
		cout << endl << "Addresses  -> Values" << endl;
		cout << "=========     ======" << endl;
		for (int it = 0; it < memory_ptr; it++) {
			cout << &(memory[it]) << "   ->   " << memory[it] << endl;
		}

		file.close();
	}

	cout << endl << endl << "**************************************************************************************" << endl;
	return 0;
}
