
//extern class twsal_function;



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



class twsal
{
    //    int parse_func(
    public:
        twsal();
	twsal(const string name, const string code, twsal * parent);
        ~twsal();
        int open(string file_name); // returns >0 if failed (can't open file, script is loced)
        int load(string script);    // as above
        void close();
        string execute();
	string execute_safe();
        string script();
	   void set_var(string name, string what);
	   string get_var(string name);
	   void set_array(string name, string num, string what);
	   string get_array(string name, string num);
	string name;	   
	string meat, actual;    // mhm... our meat ;)
//	static const string endfunction /*= "_%!END!"*/;// = "_%!END!";
	   
    protected:   
    	int curr_line;
	bool root;
    	steering last_oper;
	string func_result;
	void parse_for(string src, string *a1, string *a2, string *a3);
        string trim(string src);
	int next_bracket(int from, int * curr_line_add);
	steering parse_steer(const string src, string *for_exec, string *repeat_check);
	   string trim_br(string src);
        string parse_func(/*vector< vector<string> > *dest,*/ string src, bool * terminate = NULL);
	   string parse_line(string src, bool * terminate = NULL);	
        string value(string what, bool * terminate = NULL);	
	   int strtoint(string what);
	   string inttostr(int what);
	   string do_operator(string src1, string src2, string no_val, string oper);
	twsal * parent;
   // private:
        //string meat, actual;    // mhm... our meat ;)
        int pos, meat_size;
        bool is_comment, is_string;
	int is_pale;
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
        vector<twsal> functions;
	vector<twsal> functions_safe;
       // vector<int> *book_valgues;
       
       /* class twsal_function : twsal
        {
             public:
                  string name;
                  // vector<twsal_function> relative_functions;  <-- TODO!!
                  string code;
        }; */
};








