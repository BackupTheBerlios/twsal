


bool check_char(char ch)
{
//	char e[] = {ch, 0};
// 	static char av_chars[] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890_-";
// 	static int len = strlen(av_chars);
// 	for (int f = 0; f < len; f++)
// 	{
// 		if (av_chars[f] == ch)
// 			return true;
// 	}
// 	return false
	char e[] = {ch, 0};
	static char av_chars[] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890_-";
	if (strstr(av_chars, e))
	{
		return true;
	}else
		return false;
}

int name_end(int from, int s, string & in)
{
	int to = 0;
	for (int i = from; i < s; i++)
	{
		if (check_char(in[i]))
		{
			to++;
		}else
			break;
	}
	return to;
}

string encode_string(string in, vector<string> &names, vector<string> &values)
{
	string out;
	int s = in.size();
	bool last = false;
	for (int i = 0; i < s; i++)
	{
		if ((in[i] == '\\') && (!last))
		{
			last = true;
		}else
		{
			if (last)
			{
				if (in[i] ==  'n')
				{
					out += '\n';
				}else
				if (in[i] == '\\')
				{
					out += '\\';
				}else
				if (in[i] == 't')
				{
					out += '\t';
				}else
					out += in[i];
				last = false;
			}else
			{
				if (in[i] != '$')
				{
					out += in[i];
				}else
				{
					int s2 = names.size();
					string name = string(in, i+1, name_end(i+1, s, in));
					for (int q = 0; q < s2; q++)
					{
						if (name == names[q])
						{
							out += values[q];
							i += name.size();
							break;
						}
					}
				}
			}
		}
	}
	return out;
}
