
enum steering {
	OMMIT,
	EXEC,
	REPEAT_IF,
	REPEAT_IF_DO,
	FUNCTION
};

enum loop_type {
	FOR,
	WHILE
};

enum param_type {
	VARIABLE,
	ARRAY
};

struct twsal_memory
{
	inline twsal_memory(int curr_line = 0)
	{
		pos = 0;  //actual position of parser
		//is_comment = false;
		//is_string = false; 
		//is_pale = 0;
		last_oper = (steering)NULL;
		this->curr_line = curr_line;
	}
	int curr_line;
	int pos;
	//bool is_comment, is_string;
	//int is_pale;
	vector<int> stairs; // stack with addresses of last bracket in { } clauses
	vector<int> stairs_back;	// stack with addresses of first brackets in { } clauses
	vector<string> stairs_if; // stack with conditions for 'while' loops
	vector<string> stairs_for; // stack with code to be executed in 'for' loops
	vector<loop_type> stairs_types;
	vector<string> var_names;
	vector<string> var_values;
	vector<string> book_names;
	vector< vector<string> > command;
	vector<string> array_names;
	vector< vector<string>* > array_values;
	vector<int> book_values;
	string actual;
	steering last_oper;
	string func_result;	
};


class twsal
{
	public:
		twsal();
		twsal(const string name, const string code, twsal * parent);
		~twsal();
		int open(string file_name); // returns >0 if failed (can't open file, script is loced)
		int load(string script);    // as above
		void close();
		string execute(twsal_memory * _mem = NULL);
		inline string execute(vector<string> * params, vector<param_type> & types, twsal_memory * parentmem)
		{
			memory.push_back(* new twsal_memory());
			twsal_memory * mem = &memory.back();
			for (int i = 0 /*0. arg. is func. name*/; i < argnames.size(); i++)
			{
				if (i <= (params->size()))
				{
					if (types[i] == VARIABLE)
					{
						set_var(argnames[i], (*params)[i+1], mem);
					}else
					if (types[i] == ARRAY)
					{
						mem->array_names.push_back(argnames[i]);
						//mem->array_names.push_back(cutdollar((*params)[i+1]));
						//mem->array_values.push_back(find_array(parentmem, mem->array_names.back()));
						mem->array_values.push_back(find_array(parentmem, cutdollar((*params)[i+1])));
					}
				}	
			}
			return execute(mem);
		}
		string execute_safe();
		vector<string> * find_array(twsal_memory * mem, string  name)
		{
			int s = mem->array_names.size();
			for (int i = 0; i < s; i++)
			{
				if (mem->array_names[i] == name)
				{
					return mem->array_values[i];
				}
			}
			return NULL;
		}
		string script();
		void set_var(string name, string what, twsal_memory * mem);
		string get_var(string name, twsal_memory * mem);
		void set_array(string name, string num, string what, twsal_memory * mem);
		string get_array(string name, string num, twsal_memory * mem);
		bool exists(twsal_memory & mem, param_type type, string * name);
		string name;	   
		string meat;

		string cutdollar(string & src)
		{
			if (src[0] == '$')
			{
				return string(src, 1, src.size());
			}else
				return src;
		}
		
	protected:   
		bool root;
		void parse_for(string src, string *a1, string *a2, string *a3);
		string trim(string src);
		int next_bracket(int from, int * curr_line_add);
		steering parse_steer(const string src, string *for_exec, string *repeat_check, twsal_memory * mem);
		string trim_br(string src);
		string parse_func(/*vector< vector<string> > *dest,*/ string src, twsal_memory * mem, bool * terminate = NULL);
		string parse_line(string src, twsal_memory * mem, bool * terminate = NULL);	
		string value(string what, twsal_memory * mem, bool * terminate = NULL);	
		int strtoint(string what);
		string inttostr(int what);
		string do_operator(string src1, string src2, string no_val, string oper, twsal_memory * mem);
		twsal * parent;
		vector<string> argnames;
		int /*pos,*/ meat_size;
		vector<twsal> functions;
		vector<twsal> functions_safe;
		vector<twsal_memory> memory;
};

// vim: ts=5
