#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdlib.h>

using namespace std;

#include "twsal.h"
#include "encode_string.h"

twsal::twsal()
{
	memory.push_back(* new twsal_memory());
	twsal_memory * mem = & memory.back();
	mem->pos = 0;  //mem->actual mem->position of parser
	//is_comment = false;
	//is_string = false; 
	//is_pale = 0;
	mem->last_oper = (steering)NULL;
	mem->curr_line = 0;
	parent = NULL;
	root = true;
//	memory = new vector<twsal_memory>;
}


twsal::twsal(const string name, const string code, twsal * parent)
{
	twsal();
	this->name = name;
	this->parent = parent;
	meat = code;
	meat_size = meat.length();
	root = false;
}

twsal::~twsal()
{

}


int twsal::open(string file_name)
{
	string temp, file;
	ifstream in(file_name.c_str());
	while(getline(in, temp))
		file += temp + "\n";
	//cout << temp;
	load(file);
	//return 0;
}

int twsal::load(string script)
{
	meat = script;
	meat_size = meat.length();
	return 0;
}

string twsal::script()
{
	return meat;
}

string twsal::value(string what, twsal_memory * mem, bool * terminate/* = NULL*/)
{
	if ((what[0] == '$') && (what[what.length()-1] == ']'))
	{
		// set_array("$ah", "2", "bzz");
		//  return get_array("$ah", "2");
		return get_array(string(what, 0, what.find("[")), parse_line(string(what, what.find("[")+1, what.length()-what.find("[")-2 ), mem), mem);
	}else
	if (what[0] == '$')
	{
		return get_var(what, mem);
	}else
	if ((what[0] == '"') && (what[what.length()-1] == '"'))
	{
	// cout << "NAH! " << what;
		return encode_string(string(what, 1, what.length()-2), mem->var_names, mem->var_values);
	}else
	if (what[0] == '(')
	{
		return parse_line(string(what, 1, what.length()-2), mem);
	}else
	if ((what[0]=='1')||(what[0]=='2')||(what[0]=='3')||(what[0]=='4')
	||(what[0]=='5')||(what[0]=='6')||(what[0]=='7')||(what[0]=='8')||
	(what[0]=='9')||(what[0]=='0'))
	{
		return what;
	}else
	if (what[what.length()-1] == ')')
	{
		return parse_func(/*&mem->command,*/ what, mem, terminate);
	}else
	{
		cout << mem->curr_line << ":Parse error! " << what << "\n";
		return what;
	}
	//return what;
}

string twsal::do_operator(string src1, string src2, string no_val, string oper, twsal_memory * mem)
{
	if (oper == ".")
	{
		return src1+value(src2, mem);
	}else
	if (oper == "+")
	{
		return inttostr(strtoint(src1)+strtoint(value(src2, mem)));
	}else
	if (oper == "-")
	{
		return inttostr(strtoint(src1)-strtoint(value(src2, mem)));
	}else		     
	if (oper == "*")
	{
		return inttostr(strtoint(src1)*strtoint(value(src2, mem)));
	}else
	if (oper == "^")
	{
		return inttostr(strtoint(src1)^strtoint(value(src2, mem)));
	}else		     
	if (oper == "++")
	{
		set_var(no_val, inttostr(strtoint(src1)+1), mem);
		return get_var(no_val, mem);
	}else
	if (oper == "--")
	{
		set_var(no_val, inttostr(strtoint(src1)-1), mem);
		return get_var(no_val, mem);
	}else
	if (oper == "=")
	{
		set_var(no_val, value(src2, mem), mem);
		return "";
	}else	     
	if (oper == "==")
	{
		if (src1 == value(src2, mem))
		{
			return "1"; 
	}else
		{
			return "";
		}   
	}else
	if (oper == "!=")
	{
		if (src1 != value(src2, mem))
		{
			return "1"; 
		}else
		{
			return "";
		}
	}else	
	if (oper == ">")
	{
		if (strtoint(src1) > strtoint(value(src2, mem)))
		{
			return "1";
		}else
			return "";
	}else
	if (oper == "<")
	{
		if (strtoint(src1) < strtoint(value(src2, mem)))
		{
			return "1";
		}else
			return "";
	}else
	{
		cout << mem->curr_line << ":Error, unknown operator " << oper << ".";
	}    
}

string twsal::parse_line(string src, twsal_memory * mem, bool * terminate)
{
	src = trim(src);
	int i;
	string current = "";
	string last = "";
	string last_operator = "";
	string last_no_value = "";
	string current_operator = "";
	bool t_string = false;
	int t_pale = 0;
	int t_array = 0;
	for (i = 0; i < src.length(); i++)
	{
	//if (!((src[i] == ' ') /*&& (src[i] == '\t')*/ && (!t_string)))
	if (((src[i] != ' ') && (src[i] != '\t')) || t_string)
		{
		if ((src[i] == '(') && (!t_string))
		{
			t_pale++;
		}else
		if ((src[i] == '[') && (!t_string))
		{
			t_array++;
		}else            
		if ((src[i] == ')') && (!t_string))
		{
			t_pale--;
		}else
		if ((src[i] == ']') && (!t_string))
		{
			t_array--;
		}else            
		if (src[i] == '"')
		{
			t_string = !t_string;
	
		}
	
		if (((src[i] == '.') || (src[i] == '+') || (src[i] == '-') || (src[i] == '=')
		|| (src[i] == '^') || (src[i] == '!') || (src[i] == '<') || (src[i] == '>')
		|| (src[i] == '*') || (src[i] == '/')) && (!t_string) && (t_pale == 0) && (t_array == 0))
		{
			current_operator += src[i];
			
			//last_operator = src[i];
		}else
		{
			if (current_operator != "")
			{
				if (last_operator != "")
				{ 
					last = do_operator(last, current, last_no_value, last_operator, mem);
					//mem->actual = current;
					current = "";
				}else
				{
				last_no_value = current;
				last = value(current, mem, terminate);
				current = "";
				}	
				last_operator = current_operator;
				current_operator = "";	              
			}
			current += src[i];
		} 
	
		}      
		
	}
	if (last_operator != "")
	{
		last = do_operator(last, current, last_no_value, last_operator, mem);
	}	  
	if (current_operator != "")
	{
		// last = do_operator(last, "", last_no_value, current_operator);
		last = do_operator(value(current, mem, terminate), "", current, current_operator, mem);
	}
	if ((last == "") && (current_operator == "") && (last_operator == ""))
	{
		last = value(current, mem, terminate);
		current = "";
	}    
	return last;  
    
}

void twsal::set_var(string name, string what, twsal_memory * mem)
{
	string temp;
	if (name[0] == '$')
	{
		temp = string(name, 1, name.length()-1);
	}else
	{
		temp = name;
	}
	
	if (temp[temp.length()-1] == ']')
	{
		set_array(string(temp, 0, temp.find("[")), parse_line(string(temp, temp.find("[")+1, temp.length()-temp.find("[")-2 ), mem), what, mem);
		return;
	}
	
	for (int i = 0; i < mem->var_names.size(); i++)
	{
		if (temp == mem->var_names[i])
		{
			mem->var_values[i] = what;
			return;
		}
	}      
	mem->var_names.push_back(temp);
	mem->var_values.push_back(what);
}

void twsal::set_array(string name, string num, string what, twsal_memory * mem)
{
	string temp;
	if (name[0] == '$')
	{
		temp = string(name, 1, name.length()-1);
	}else
	{
		temp = name;
	}
	for (int i = 0; i < mem->array_names.size(); i++)
	{
		if (temp == mem->array_names[i])
		{
			if (strtoint(num) >= mem->array_values[i].size())
			{
				int temp2 = (strtoint(num)-1);
				while (mem->array_values[i].size() <= temp2)
				{
					mem->array_values[i].push_back("");
				}
				mem->array_values[i].push_back(what);
			}else
			{
				mem->array_values[i][strtoint(num)] = what;
			}
			return;
		}
	}
	mem->array_names.push_back(temp);
	mem->array_values.push_back(*(new vector<string>)); 
	
	int size = mem->array_values.size()-1;
	if (strtoint(num) >= mem->array_values[size].size())
	{
		int temp2 = (strtoint(num));
		while (mem->array_values[size].size() < temp2)
		{
			mem->array_values[size].push_back("");
		}
		mem->array_values[size].push_back(what);
	}else
	{
		mem->array_values[size][strtoint(num)] = what;
	}           
}

string twsal::get_var(string name, twsal_memory * mem)
{
	string temp;
	if (name[0] == '$')
	{
		temp = string(name, 1, name.length()-1);
	}else
	{
		temp = name;
	}
	for (int i =0; i < mem->var_names.size(); i++)
	{
		if (temp == mem->var_names[i])
		{
			return mem->var_values[i];
		}
	}
	if (root == false)
	{
		return parent->get_var(name, mem);
	}else
	{
		return "";
	}
}

string twsal::get_array(string name, string num, twsal_memory * mem)
{
	string temp;
	int tmpsize = strtoint(num);
	if (name[0] == '$')
	{
		temp = string(name, 1, name.length()-1);
	}else
	{
		temp = name;
	}
	for (int i = 0; i < mem->array_names.size(); i++)
	{
		if (temp == mem->array_names[i])
		{	
			if (mem->array_values[i].size() > tmpsize)
			{
				return mem->array_values[i][tmpsize];
			}else
				return "";
		}
	}
	return "";
}

string twsal::parse_func(/*vector< vector<string> > *dest, */string src, twsal_memory * mem, bool  *terminate)
{
	int owned; // która czê¶æ mem->command jest do naszej dyspozycji
	src = trim(src);
	bool t_string = false;
	int t_pale = 0;
	int t_array = 0;
	mem->command.push_back(*(new vector<string>));
	owned = mem->command.size()-1;
	int i;
	string temp;
	for (i = 0; i < src.length(); i++)
	{
		if ((src[i] != ' ') && (src[i] != '\t'))  // ignore spaces       
		if (src[i] == '(')
		{
			mem->command[owned].push_back(temp); // mamy juz nazwe funkcji
			break;   
		}else
		{
			temp += src[i]; 
		}          
	}
	temp = "";
	for (i = (i+1); i < src.length(); i++)
	{
		if ((src[i] == '(') && (!t_string))
		{
			t_pale++;
		}else
		if ((src[i] == '[') && (!t_string))
		{
			t_array++;
		}else         
		if ((src[i] == ')') && (t_pale > 0) && (!t_string))
		{
			t_pale--;
		}else
		if ((src[i] == ']') && (t_array > 0) && (!t_string))
		{
			t_array--;
		}else         
		if (src[i] == '"')
		{
			t_string = !t_string;
		}else
		if ((src[i] == ')') && (t_pale == 0) && (t_array == 0) && (!t_string))
		{
			mem->command[owned].push_back(temp);
			break;
		}
		
		if ((src[i] == ',') && (!t_pale) && (!t_array) && (!t_string))
		{
			//temp += src[i];
			mem->command[owned].push_back(temp);
			temp = "";
		}else         
			temp += src[i];
	}
	
	// FIXME: this is not too efficient...
	if (mem->command[owned].back() == "")
	{
		mem->command[owned].pop_back();
	}
	
	if (mem->command[owned][0] == "echo")
	{
		cout << parse_line(mem->command[owned][1], mem);
		mem->command[owned].pop_back();
		return "";
	}else
	if (mem->command[owned][0] == "return")
	{
		if (terminate != NULL)
		{
			(*terminate) = true;
		}
		if (mem->command[owned].size() >= 2)
		{
			return parse_line(mem->command[owned][1], mem);
		}else
			return "";
	}else
	if (mem->command[owned][0] == "length")
	{
		int templen = mem->array_names.size();
		if (!(mem->command[owned].size() >= 2))
		{
			cout << "\n" << mem->curr_line << ": Error: length() - You must give a name of a table.\n";
		}
		for (int r = 0; r < templen; r++)
		{
			if (mem->array_names[r] == mem->command[owned][1])
			{
				return inttostr(mem->array_values[r].size());
			}
		}
		return "0";
	}
	if (mem->command[owned][0] == "substr")
	{
		mem->command[owned].pop_back(); 
		return string(parse_line(mem->command[owned][1], mem), strtoint(parse_line(mem->command[owned][2], mem)), strtoint(parse_line(mem->command[owned][3], mem)));
	}
	if (mem->command[owned][0] == "get")
	{
		string temp;
		getline(cin, temp);
		mem->command[owned].pop_back();
		return temp;
	}else
	{
	// TODO TODO TODO TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		for (int y = 0; y < functions.size(); y++)
		{
			if (mem->command[owned][0] == functions[y].name)
			{
				
				//for (int q = 1; q < mem->command[owned].size(); q++)
				//{
				//	if (q-1 < functions[y].mem->var_names.size())
				//	{
				//		functions[y].mem->var_values[q-1] = parse_line(mem->command[owned][q]);
				//	}
				//}
				//for (int q = 0; q < functions[y].argnames.size(); q++)
				//{
				//	if ((q+1) >= mem->command[owned].size())
				//	{
				//		command
				//	}
				//}
				int llen = mem->command[owned].size();
				for (int g = 1; g < llen; g++)
				{
					mem->command[owned][g] = parse_line(mem->command[owned][g], mem);
				}
				return functions[y].execute(& mem->command[owned]);
				//return functions[y].execute();
			}
		}
		if (!root)	
			return parent->parse_func(src, mem, terminate);
		//}
		cout << '\n' << mem->curr_line << ": Unknown function " << mem->command[owned][0] << "!!\n";
		mem->command[owned].pop_back();
		return "";
	}
	
}


string twsal::execute_safe()
{
	functions_safe.push_back(*(new twsal(this->name, this->meat, this->parent)));
	string result = functions_safe.back().execute();
	functions_safe.pop_back();
	return result;
}


void twsal::parse_for(string src, string *a1, string *a2, string *a3)
{
	bool is_string = false;
	int count = src.length();
	int level = 0;
	//if ((src[0] == '(') && (src[src.length()-1] == ')'))
	//{
	//	src[0] = ' ';
	//	src[src.length()-1] = ' ';
	//}
	//parse_line("$j = 0");
	//(*a1) = "$j = 0";
	//(*a2) = "$j < 10";
	//(*a3) = "$j = ($j + 1)";
	
	for (int i = 1; i < (count-1); i++)
	{
		if ((src[i] == ';') && !is_string && (level < 2))
		{
			level++;
			//continue;
		}else
		{
			if (src[i] == '"')
			{
				is_string = !is_string;
			}
			if (level == 0)
			{
				(*a1) += src[i];
			}else
			if (level == 1)
			{
				(*a2) += src[i];
			}else
			if (level == 2)
			{
				(*a3) += src[i];
			}
			/*switch (level)
			{
				case 0 : (*a1) += src[i];
				case 1 : (*a2) += src[i];
				case 2 : (*a3) += src[i];
			}*/
		}
	}
}



steering twsal::parse_steer(const string src, string *for_exec, string *repeat_check, twsal_memory * mem)
{
	string curr = "";
	//string what = trim(trim_br(src));
	string what = trim(src);
	int count = what.length();
	int i;
	for (i = 0; i< count; i++)
	{
		if ((what[i] != ' ') && (what[i] != '\t') && (what[i] != '\n'))
		if ((what[i] == '(') || (what[i] == '{'))
		{
			if (curr == "if")							
			{ 									
				if ((parse_line(string(what, i, count-i), mem) != "0") && (parse_line(string(what, i, count-i), mem) != ""))		
				{								
					return EXEC;						
				}else								
					return OMMIT;						
			}else									
			if (curr == "elif")							
			{									
				if (mem->last_oper == OMMIT)						
				{								
					if ((parse_line(string(what, i, count-i), mem) != "0") && (parse_line(string(what, i, count-i), mem) != ""))
					{							
						return EXEC;					
					}else							
					{							
						return OMMIT;					
					}							
				}else								
				{								
					return OMMIT;						
				}							
			}else
			if (curr == "while")
			{
				if (repeat_check != NULL)
				{
					(*repeat_check) = string(what, i, count-i);
					return REPEAT_IF;
				}
			}else
			if (curr == "for")
			{
				// TODO !!!
				if ((repeat_check != NULL) && (for_exec != NULL))
				{
					string a1 = "";
					string a2 = "";
					string a3 = "";
					parse_for(string(what, i, count-i), &a1, &a2, &a3);
					parse_line(a1, mem);
					(*for_exec) = a3;
					(*repeat_check) = a2;
					return REPEAT_IF_DO;
				}
			}else
			if (curr[0] == '_')
			{
				//cout << "bum!";
				string tmpname = trim(string(curr, 1, curr.length()));
				int fcount = functions.size();
				for (int r = 0; r < fcount; r++)
				{
					if (functions[r].name == tmpname)
					{
						return OMMIT;
					}
				}
				functions.push_back(*(new twsal(tmpname, trim(string(meat, mem->pos+1, next_bracket(mem->pos, (int*)NULL)-mem->pos-1)), this)));
				string arguments = string(what, i+1, count-i-2);
				int arglen = arguments.length();
				string tmparg;
				int q;
				for (q = 0; q < arglen; q++)
				{
					if ((arguments[q] != ' ') && (arguments[q] != '\t'))
					if (arguments[q] == ',')
					{
						//functions.back().set_var(tmparg, "", mem);
						functions.back().argnames.push_back(tmparg);
						tmparg = "";
					}else
					{
						tmparg += arguments[q];
					}
				}
				if (arglen > 0)
				{
					functions.back().argnames.push_back(tmparg);
				}
				return OMMIT;
			}
			#define CHECKER 							\
			if (curr == "else")							\
			{									\
				if (mem->last_oper == OMMIT)						\
				{								\
					return EXEC;						\
				}else								\
				{								\
					return OMMIT;						\
				}								\
			}else									\
			{									\
				cout << mem->curr_line << ":What did you mean by " << curr << "?\n";	\
			}
			#ifdef CHECKER
			else
			#endif
			CHECKER
		}else
		{
			curr += what[i];
		}
	}
	CHECKER

}

// Takes mem->position of {, returns mem->position of corresponding }.
int twsal::next_bracket(int from, int * curr_line_add)
{
	int brackets = 1;
	bool is_comment = false;
	bool is_string = false;
	//int count = meat.length();
	for (int i = from+1; i < meat_size; i++)
	{
		if ((meat[i] == '"') && (!is_comment))
		{
			is_string = !is_string;
		}else
		if ((meat[i] == '#') && (!is_string) && (!is_comment))
		{
			is_comment = true;
		}else
		if ((meat[i] == '\n') && (!is_string) && (is_comment))
		{
			is_comment = false;
			if (curr_line_add != NULL)
				(*curr_line_add)++;
		}else
		if ((meat[i] == '{') && (!is_string) && (!is_comment))
		{
			brackets++;
		}else
		if ((meat[i] == '}') && (!is_string) && (!is_comment))
		{
			brackets--;
		}else
		if (meat[i] == '\n')
		{
			if (curr_line_add != NULL)
				(*curr_line_add)++;
		}

		if (brackets == 0)
		{
			return i;
		}
	}
}


string twsal::execute(twsal_memory * _mem)
{
	bool is_string = false;
	bool is_comment = false;
	int is_pale = 0;
	twsal_memory * mem;
	if (_mem == NULL)
	{
		memory.push_back(*new twsal_memory());
		mem = &memory.back();
	}else
	{
		mem = _mem;
	}
//	mem->pos = 0;  //mem->actual mem->position of parser
//	is_comment = false;
//	is_string = false; 
//	is_pale = 0;
//	mem->last_oper = (steering)NULL;
	while (mem->pos < meat_size)
	{
		if ( is_comment || is_string || is_pale || ((meat[mem->pos] != ';') && (meat[mem->pos] != '{') && (meat[mem->pos] != '}')))
		{
			if ((meat[mem->pos] == '"') && (!is_comment))
			{
				is_string = !is_string;
				mem->actual += meat[mem->pos];
			}else
			if ((meat[mem->pos] == '(') && (!is_comment) && (!is_string))
			{
				is_pale++;
				mem->actual += meat[mem->pos];
			}else
			if ((meat[mem->pos] == ')') && (!is_comment) && (!is_string))
			{
				is_pale--;
				mem->actual += meat[mem->pos];
			}else
			if ((meat[mem->pos] == '#') && (!is_string) && (! is_comment))
			{ 
				is_comment = true;                                
			}else        
			if ((meat[mem->pos] == '\n') && (!is_string) && (is_comment))
			{
				is_comment = false;     
				mem->curr_line++;
			}else
			{
				if (! is_comment)
				{
					mem->actual += meat[mem->pos];
					if (meat[mem->pos] == '\n')
						mem->curr_line++;
				}	
			}
		}else  
		if (meat[mem->pos] == ';')
		{
			//if (! is_pale
			bool terminate = false;
			string com_result = parse_line(trim_br(trim(trim_br(mem->actual))), mem, &terminate);
			if (terminate == true)
			{
				return com_result;
			}
			mem->actual = "";
		}else
		if (meat[mem->pos] == '{')
		{
			mem->stairs_if.push_back("");
			mem->stairs_for.push_back("");
			mem->last_oper = parse_steer(mem->actual, &mem->stairs_for[mem->stairs_for.size()-1], &mem->stairs_if[mem->stairs_if.size()-1], mem);
			if (mem->last_oper == EXEC)
			{	
				mem->actual = "";
				//continue;
				mem->stairs_if.pop_back();
				mem->stairs_for.pop_back();
			}else
			if (mem->last_oper == OMMIT)
			{
				mem->pos = next_bracket(mem->pos, &mem->curr_line)+1;
				mem->actual = ""; 
				mem->stairs_if.pop_back();
				mem->stairs_for.pop_back();
				continue;
			}else
			if (mem->last_oper == REPEAT_IF)
			{
				string tmp = parse_line(mem->stairs_if[mem->stairs_if.size()-1], mem);
				if ((tmp == "0")  || (tmp == ""))
				{
					mem->pos = next_bracket(mem->pos, &mem->curr_line)+1;
					mem->actual = "";
					mem->stairs_if.pop_back();
					mem->stairs_for.pop_back();
					continue;
				}
				mem->stairs.push_back(next_bracket(mem->pos, NULL));
				mem->stairs_back.push_back(mem->pos);
				mem->actual = "";
				mem->stairs_for.pop_back();
				mem->stairs_types.push_back(WHILE);
			}else
			if (mem->last_oper == REPEAT_IF_DO)
			{
				string tmp = parse_line(mem->stairs_if[mem->stairs_if.size()-1], mem);
				if ((tmp == "0") || (tmp == ""))
				{
					mem->pos = next_bracket(mem->pos, &mem->curr_line)+1;
					mem->actual = "";
					mem->stairs_if.pop_back();
					mem->stairs_for.pop_back();
					continue;
				}
				mem->stairs.push_back(next_bracket(mem->pos, NULL));
				mem->stairs_back.push_back(mem->pos);
				mem->actual = "";
				mem->stairs_types.push_back(FOR);
			}
		}else
		if (meat[mem->pos] == '}')
		{
			if (mem->stairs.size() > 0)
			if (mem->pos == mem->stairs[mem->stairs.size()-1])
			{
				if (mem->stairs_types[mem->stairs_types.size()-1] == WHILE)
				{
					string tmp = parse_line(mem->stairs_if[mem->stairs_if.size()-1], mem);
					if ((tmp != "0") && (tmp != ""))
					{
						mem->pos = mem->stairs_back[mem->stairs_back.size()-1]+1;
						continue;
					}else
					{
						mem->stairs.pop_back();
						mem->stairs_if.pop_back();
						mem->stairs_back.pop_back();
						mem->stairs_types.pop_back();
						mem->actual = "";
						continue;
					}
				}else
				if (mem->stairs_types[mem->stairs_types.size()-1] == FOR)
				{
					parse_line(mem->stairs_for[mem->stairs_for.size()-1], mem);
					string tmp = parse_line(mem->stairs_if[mem->stairs_if.size()-1], mem);
					if ((tmp != "0") && (tmp != ""))
					{
						mem->pos = mem->stairs_back[mem->stairs_back.size()-1]+1;
						continue;
					}else
					{
						mem->stairs.pop_back();
						mem->stairs_if.pop_back();
						mem->stairs_back.pop_back();
						mem->stairs_types.pop_back();
						mem->stairs_for.pop_back();
						mem->actual = "";
						continue;
					}
				}
			}
			mem->actual = "";
		}
		
		mem->pos++;
	}
	//mem->pos = 0;
	memory.pop_back();
	return "";
}



string twsal::trim(string src)
{
	int len = src.length();
	int from = 0;
	int to = 0;
	int i;
	for (i = 0; i < len; i++)
	{
		if ((src[i] == ' ') || (src[i] == '\t') || (src[i] == '\n'))
		{
			from++;
		}else
			break;
	}
	for (i = len-1; i >= 0; i--)
	{
		if ((src[i] == ' ') || (src[i] == '\t') || (src[i] == '\n'))
		{
			to++;
		}else
			break;
	}
	return string(src, from, len-from-to);
}

string twsal::trim_br(string src)
{
	if(src.length() == 0)
		return src;
	int b = src.find_first_not_of("\n");
	int e = src.find_last_not_of("\n");
	return std::string(src, b, e - b + 1);
}

int twsal::strtoint(string what)
{
	int thing;
	stringstream(what) >> thing;
	return thing;    
}

string twsal::inttostr(int what)
{
	string thing;
	stringstream convertor;
	convertor << what;
	convertor >> thing;
	return thing;
}

// vim: ts=5
