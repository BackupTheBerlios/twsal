#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdlib.h>

using namespace std;

#include "twsal.h"

twsal::twsal()
{
	pos = 0;  //actual position of parser
	is_comment = false;
	is_string = false; 
	is_pale = 0;
	locked = false;
	last_oper = (steering)NULL;
	curr_line = 0;
	parent = NULL;
	root = true;
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
	if (locked == false)
	{
		meat = script;
		meat_size = meat.length();
		return 0;
	}else
	{
		return 1;
	}        
}

string twsal::script()
{
	return meat;
}

string twsal::value(string what, bool * terminate/* = NULL*/)
{
	if ((what[0] == '$') && (what[what.length()-1] == ']'))
	{
		// set_array("$ah", "2", "bzz");
		//  return get_array("$ah", "2");
		return get_array(string(what, 0, what.find("[")), parse_line(string(what, what.find("[")+1, what.length()-what.find("[")-2 )));
	}else
	if (what[0] == '$')
	{
		return get_var(what);
	}else
	if ((what[0] == '"') && (what[what.length()-1] == '"'))
	{
	// cout << "NAH! " << what;
		return string(what, 1, what.length()-2);
	}else
	if (what[0] == '(')
	{
		return parse_line(string(what, 1, what.length()-2));
	}else
	if ((what[0]=='1')||(what[0]=='2')||(what[0]=='3')||(what[0]=='4')
	||(what[0]=='5')||(what[0]=='6')||(what[0]=='7')||(what[0]=='8')||
	(what[0]=='9')||(what[0]=='0'))
	{
		return what;
	}else
	if (what[what.length()-1] == ')')
	{
		return parse_func(/*&command,*/ what, terminate);
	}else
	{
		cout << curr_line << ":Parse error! " << what << "\n";
		return what;
	}
	//return what;
}

string twsal::do_operator(string src1, string src2, string no_val, string oper)
{
	if (oper == ".")
	{
		return src1+value(src2);
	}else
	if (oper == "+")
	{
		return inttostr(strtoint(src1)+strtoint(value(src2)));
	}else
	if (oper == "-")
	{
		return inttostr(strtoint(src1)-strtoint(value(src2)));
	}else		     
	if (oper == "*")
	{
		return inttostr(strtoint(src1)*strtoint(value(src2)));
	}else
	if (oper == "^")
	{
		return inttostr(strtoint(src1)^strtoint(value(src2)));
	}else		     
	if (oper == "++")
	{
		return inttostr(strtoint(src1)+1); 
	}else
	if (oper == "=")
	{
		set_var(no_val, value(src2));
		return "";
	}else	     
	if (oper == "==")
	{
		if (src1 == value(src2))
		{
			return "1"; 
	}else
		{
			return "";
		}   
	}else
	if (oper == "!=")
	{
		if (src1 != value(src2))
		{
			return "1"; 
		}else
		{
			return "";
		}
	}else	
	if (oper == ">")
	{
		if (strtoint(src1) > strtoint(value(src2)))
		{
			return "1";
		}else
			return "";
	}else
	if (oper == "<")
	{
		if (strtoint(src1) < strtoint(value(src2)))
		{
			return "1";
		}else
			return "";
	}else
	{
		cout << curr_line << ":Error, unknown operator " << oper << ".";
	}    
}

string twsal::parse_line(string src, bool * terminate)
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
					last = do_operator(last, current, last_no_value, last_operator);
					//actual = current;
					current = "";
				}else
				{
				last_no_value = current;
				last = value(current, terminate);
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
		last = do_operator(last, current, last_no_value, last_operator);
	}	  
	if (current_operator != "")
	{
		// last = do_operator(last, "", last_no_value, current_operator);
		last = do_operator(value(current, terminate), "", last_no_value, current_operator);
	}
	if ((last == "") && (current_operator == "") && (last_operator == ""))
	{
		last = value(current, terminate);
		current = "";
	}    
	return last;  
    
}

void twsal::set_var(string name, string what)
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
		set_array(string(temp, 0, temp.find("[")), parse_line(string(temp, temp.find("[")+1, temp.length()-temp.find("[")-2 )), what);
		return;
	}
	
	for (int i = 0; i < var_names.size(); i++)
	{
		if (temp == var_names[i])
		{
			var_values[i] = what;
			return;
		}
	}      
	var_names.push_back(temp);
	var_values.push_back(what);
}

void twsal::set_array(string name, string num, string what)
{
	string temp;
	if (name[0] == '$')
	{
		temp = string(name, 1, name.length()-1);
	}else
	{
		temp = name;
	}
	for (int i = 0; i < array_names.size(); i++)
	{
		if (temp == array_names[i])
		{
			if (strtoint(num) >= array_values[i].size())
			{
				int temp2 = (strtoint(num)-1);
				while (array_values[i].size() <= temp2)
				{
					array_values[i].push_back("");
				}
				array_values[i].push_back(what);
			}else
			{
				array_values[i][strtoint(num)] = what;
			}
			return;
		}
	}
	array_names.push_back(temp);
	array_values.push_back(*(new vector<string>)); 
	
	int size = array_values.size()-1;
	if (strtoint(num) >= array_values[size].size())
	{
		int temp2 = (strtoint(num));
		while (array_values[size].size() < temp2)
		{
			array_values[size].push_back("");
		}
		array_values[size].push_back(what);
	}else
	{
		array_values[size][strtoint(num)] = what;
	}           
}

string twsal::get_var(string name)
{
	string temp;
	if (name[0] == '$')
	{
		temp = string(name, 1, name.length()-1);
	}else
	{
		temp = name;
	}
	for (int i =0; i < var_names.size(); i++)
	{
		if (temp == var_names[i])
		{
			return var_values[i];
		}
	}
	if (root == false)
	{
		return parent->get_var(name);
	}else
	{
		return "";
	}
}

string twsal::get_array(string name, string num)
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
	for (int i = 0; i < array_names.size(); i++)
	{
		if (temp == array_names[i])
		{	
			if (array_values[i].size() > tmpsize)
			{
				return array_values[i][tmpsize];
			}else
				return "";
		}
	}
	return "";
}

string twsal::parse_func(/*vector< vector<string> > *dest, */string src, bool  *terminate)
{
	int owned; // która czê¶æ command jest do naszej dyspozycji
	src = trim(src);
	bool t_string = false;
	int t_pale = 0;
	int t_array = 0;
	command.push_back(*(new vector<string>));
	owned = command.size()-1;
	int i;
	string temp;
	for (i = 0; i < src.length(); i++)
	{
		if ((src[i] != ' ') && (src[i] != '\t'))  // ignore spaces       
		if (src[i] == '(')
		{
			command[owned].push_back(temp); // mamy juz nazwe funkcji
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
			command[owned].push_back(temp);
			break;
		}
		
		if ((src[i] == ',') && (!t_pale) && (!t_array) && (!t_string))
		{
			//temp += src[i];
			command[owned].push_back(temp);
			temp = "";
		}else         
			temp += src[i];
	}
	
	// FIXME: this is not too efficient...
	if (command[owned].back() == "")
	{
		command[owned].pop_back();
	}
	
	if (command[owned][0] == "echo")
	{
		cout << parse_line(command[owned][1]);
		command[owned].pop_back();
		return "";
	}else
	if (command[owned][0] == "return")
	{
		if (terminate != NULL)
		{
			(*terminate) = true;
		}
		if (command[owned].size() >= 2)
		{
			return parse_line(command[owned][1]);
		}else
			return "";
	}else
	if (command[owned][0] == "length")
	{
		int templen = array_names.size();
		if (!(command[owned].size() >= 2))
		{
			cout << "\n" << curr_line << ": Error: length() - You must give a name of a table.\n";
		}
		for (int r = 0; r < templen; r++)
		{
			if (array_names[r] == command[owned][1])
			{
				return inttostr(array_values[r].size());
			}
		}
		return "0";
	}
	if (command[owned][0] == "substr")
	{
		command[owned].pop_back(); 
		return string(parse_line(command[owned][1]), strtoint(parse_line(command[owned][2])), strtoint(parse_line(command[owned][3])));
	}
	if (command[owned][0] == "get")
	{
		string temp;
		getline(cin, temp);
		command[owned].pop_back();
		return temp;
	}else
	{
		//cout << "\n---- " << name << "\n" ;
		for (int y = 0; y < functions.size(); y++)
		{
		//	cout << "\n n: " << functions[y].name;
			if (command[owned][0] == functions[y].name)
			{
				//cout << "\nFOUND: " << this->name << " : " << root << " : " << src << " : " << command[owned][0] << '\n';
				for (int q = 1; q < command[owned].size(); q++)
				{
					if (q-1 < functions[y].var_names.size())
					{
						functions[y].var_values[q-1] = parse_line(command[owned][q]);
					}
				}
				//cout << "\nEREEE " << name << " : " << root << " : " << src << '\n';
				//cout << "\n" << root << "\n";

				return functions[y].execute();
			}
		}
		//cout << "\nERG " << name << " : " << root << " : " << src << '\n';
		//if (this->root == false)
		//{
			//cout << '\n' << name << " : " << root << " : " << src << '\n';
			//if (root)
			//	cout << "\n" << root << "\n";
		if (!root)	
			return parent->parse_func(src, terminate);
		//}
		cout << '\n' << curr_line << ": Unknown function " << command[owned][0] << "!!\n";
		command[owned].pop_back();
		return "";
	}
	
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



steering twsal::parse_steer(const string src, string *for_exec, string *repeat_check)
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
				if ((parse_line(string(what, i, count-i)) != "0") && (parse_line(string(what, i, count-i)) != ""))		
				{								
					return EXEC;						
				}else								
					return OMMIT;						
			}else									
			if (curr == "elif")							
			{									
				if (last_oper == OMMIT)						
				{								
					if ((parse_line(string(what, i, count-i)) != "0") && (parse_line(string(what, i, count-i)) != ""))
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
					parse_line(a1);
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
				functions.push_back(*(new twsal(tmpname, trim(string(meat, pos+1, next_bracket(pos, (int*)NULL)-pos-1)), this)));
				string arguments = string(what, i+1, count-i-3);
				int arglen = arguments.length();
				string tmparg;
				for (int q = 0; q < arglen; q++)
				{
					if ((arguments[q] != ' ') && (arguments[q] != '\t'))
					if (arguments[q] == ',')
					{
						functions.back().set_var(tmparg, "");
						tmparg = "";
					}else
					{
						tmparg += arguments[q];
					}
				}
				if (arglen > 0)
					functions.back().set_var(tmparg, "");
				return OMMIT;
			}
			#define CHECKER 							\
			if (curr == "else")							\
			{									\
				if (last_oper == OMMIT)						\
				{								\
					return EXEC;						\
				}else								\
				{								\
					return OMMIT;						\
				}								\
			}else									\
			{									\
				cout << curr_line << ":What did you mean by " << curr << "?\n";	\
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

// Takes position of {, returns position of corresponding }.
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


string twsal::execute()
{
	pos = 0;  //actual position of parser
	is_comment = false;
	is_string = false; 
	is_pale = 0;
	locked = false;
	last_oper = (steering)NULL;
	while (pos < meat_size)
	{
		if ( is_comment || is_string || is_pale || ((meat[pos] != ';') && (meat[pos] != '{') && (meat[pos] != '}')))
		{
			if ((meat[pos] == '"') && (!is_comment))
			{
				is_string = !is_string;
				actual += meat[pos];
			}else
			if ((meat[pos] == '(') && (!is_comment) && (!is_string))
			{
				is_pale++;
				actual += meat[pos];
			}else
			if ((meat[pos] == ')') && (!is_comment) && (!is_string))
			{
				is_pale--;
				actual += meat[pos];
			}else
			if ((meat[pos] == '#') && (!is_string) && (! is_comment))
			{ 
				is_comment = true;                                
			}else        
			if ((meat[pos] == '\n') && (!is_string) && (is_comment))
			{
				is_comment = false;     
				curr_line++;
			}else
			{
				if (! is_comment)
				{
					actual += meat[pos];
					if (meat[pos] == '\n')
						curr_line++;
				}	
			}
		}else  
		if (meat[pos] == ';')
		{
			//if (! is_pale
			bool terminate = false;
			string com_result = parse_line(trim_br(trim(trim_br(actual))), &terminate);
			if (terminate == true)
			{
				return com_result;
			}
			actual = "";
		}else
		if (meat[pos] == '{')
		{
			stairs_if.push_back("");
			stairs_for.push_back("");
			last_oper = parse_steer(actual, &stairs_for[stairs_for.size()-1], &stairs_if[stairs_if.size()-1]);
			if (last_oper == EXEC)
			{	
				actual = "";
				//continue;
				stairs_if.pop_back();
				stairs_for.pop_back();
			}else
			if (last_oper == OMMIT)
			{
				pos = next_bracket(pos, &curr_line)+1;
				actual = ""; 
				stairs_if.pop_back();
				stairs_for.pop_back();
				continue;
			}else
			if (last_oper == REPEAT_IF)
			{
				string tmp = parse_line(stairs_if[stairs_if.size()-1]);
				if ((tmp == "0")  || (tmp == ""))
				{
					pos = next_bracket(pos, &curr_line)+1;
					actual = "";
					stairs_if.pop_back();
					stairs_for.pop_back();
					continue;
				}
				stairs.push_back(next_bracket(pos, NULL));
				stairs_back.push_back(pos);
				actual = "";
				stairs_for.pop_back();
				stairs_types.push_back(WHILE);
			}else
			if (last_oper == REPEAT_IF_DO)
			{
				string tmp = parse_line(stairs_if[stairs_if.size()-1]);
				if ((tmp == "0") || (tmp == ""))
				{
					pos = next_bracket(pos, &curr_line)+1;
					actual = "";
					stairs_if.pop_back();
					stairs_for.pop_back();
					continue;
				}
				stairs.push_back(next_bracket(pos, NULL));
				stairs_back.push_back(pos);
				actual = "";
				stairs_types.push_back(FOR);
			}
		}else
		if (meat[pos] == '}')
		{
			if (stairs.size() > 0)
			if (pos == stairs[stairs.size()-1])
			{
				if (stairs_types[stairs_types.size()-1] == WHILE)
				{
					string tmp = parse_line(stairs_if[stairs_if.size()-1]);
					if ((tmp != "0") && (tmp != ""))
					{
						pos = stairs_back[stairs_back.size()-1]+1;
						continue;
					}else
					{
						stairs.pop_back();
						stairs_if.pop_back();
						stairs_back.pop_back();
						stairs_types.pop_back();
						actual = "";
						continue;
					}
				}else
				if (stairs_types[stairs_types.size()-1] == FOR)
				{
					parse_line(stairs_for[stairs_for.size()-1]);
					string tmp = parse_line(stairs_if[stairs_if.size()-1]);
					if ((tmp != "0") && (tmp != ""))
					{
						pos = stairs_back[stairs_back.size()-1]+1;
						continue;
					}else
					{
						stairs.pop_back();
						stairs_if.pop_back();
						stairs_back.pop_back();
						stairs_types.pop_back();
						stairs_for.pop_back();
						actual = "";
						continue;
					}
				}
			}
			actual = "";
		}
		
		pos++;
	}
	pos = 0;
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
	/*if(src.length() == 0)
		return src;
	int b = src.find_first_not_of(" ");
	int e = src.find_last_not_of(" ");
	string tmp = std::string(src, b, e - b + 1);
	b = src.find_first_not_of("\t");
	e = src.find_last_not_of("\t");
	return std::string(src, b, e - b + 1);*/
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
