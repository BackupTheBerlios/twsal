
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
	vector<int> stairs;
	vector<int> stairs_back;
	vector<string> stairs_if;
	vector<string> stairs_for;
	vector<loop_type> stairs_types;
	vector<string> var_names;
	vector<string> var_values;
	vector<string> book_names;
	vector< vector<string> > command;
	vector<string> array_names;
	vector< vector<string> > array_values;
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
		inline string execute(vector<string> * params)
		{
			memory.push_back(* new twsal_memory());
			twsal_memory * mem = &memory.back();
			for (int i = 0 /*0. arg. is func. name*/; i < argnames.size(); i++)
			{
				if (i <= (params->size()))
					set_var(argnames[i], (*params)[i+1], mem);
			}
			return execute(mem);
		}
		string execute_safe();
		string script();
		void set_var(string name, string what, twsal_memory * mem);
		string get_var(string name, twsal_memory * mem);
		void set_array(string name, string num, string what, twsal_memory * mem);
		string get_array(string name, string num, twsal_memory * mem);
		string name;	   
		string meat;
		
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
