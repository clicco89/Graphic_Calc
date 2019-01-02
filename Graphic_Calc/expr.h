#include <vector>
#include <stack>
#include <math.h>
#include <sstream>
#include <algorithm>
#include <stdexcept>

using namespace std;
#define E 2.71828182846
#define PI 3.14159265359

#pragma once
/* HELPERS */
void error(const char* cause) //throw a custom error
{
	throw invalid_argument(cause);
}
bool isNumber(string num)
{
	if (num.length() == 0) return false;
	if (num.length() == 1 && (num.at(0) == 'x' || num.at(0) == 'y')) return true;
	int dots = 0;
	int i;
	for (i = 0; i < num.size(); i++)
	{
		if (num.at(i) == '.') dots++;
		else if (!isdigit(num.at(i))) return false;
	}

	return dots < 2;
}
bool higherOrEqPrecOp(char op_a, char op_b)
{
	//Ascii table correction
	if (op_a == '*') op_a = 46;
	if (op_b == '*') op_b = 46;

	return op_a >= op_b;
} 
string searchClosedPar(string container, unsigned int &i) //Find the closer parenthesis
{
	string res = "";
	int p_opened_c = 1; //Number of opened parenthesis

	i++; //Parenthesis correction
	for(i; i < container.length() && p_opened_c > 0; i++)
	{
		char token = container.at(i);
		
		if (token == '(') p_opened_c++;
		if (token == ')') p_opened_c--;

		res += token;
	}
	if (p_opened_c > 0) error("Missing ')'!");
	i--;

	return res.substr(0, res.length() - 1);
}

/* PARSERS */
vector<string> parseInfix(string in_expr)
{
	//Empty correction
	if (in_expr.length() == 0) error("Argument empty!");

	//First negative correction
	string expr(in_expr.length() + 1, '\0');
	expr = (in_expr.at(0) == '-') ? '0' + in_expr : in_expr;

	/* SHUNTING-YARD */
	//vars
	stack<char> op_stack;
	vector<string> out_queue;

	//code
	for (unsigned int i = 0; i < expr.length(); i++)
	{
		char token = expr.at(i);

		//function check
		bool is_func = true;
		if (isalpha(token))
		{
			if (i == expr.length() - 1)
				is_func = false;
			else if (!isalpha(expr.at(i + 1)))
				is_func = false;
		}
		else is_func = false;
		
		//Case function
		if (is_func)
		{
			//get the function
			string func = "";
			while (isalpha(expr.at(i)))
			{
				func += expr.at(i);
				i++;
				if (i == expr.length()) error("Missing '('!");
			}
			if (expr.at(i) != '(')
				error("Missing '('!");

			//get the argument
			string argument = searchClosedPar(expr, i); //Function argument

			//parse function
			vector<string> postfix_arg = parseInfix(argument);
			out_queue.push_back(func);
			out_queue.push_back(to_string(postfix_arg.size()));
			out_queue.insert(out_queue.end(), postfix_arg.begin(), postfix_arg.end());

		}
		else
		{
			//Case constant/variable
			if (token == 'x' || token == 'e' || token == 'p')
			{
				out_queue.push_back(string(1, token));
			}
			//Case number
			else if ((token - '0') >= 0 && (token - '0') <= 9)
			{
				string final_num = "";
				do
				{
					final_num += expr.at(i);
					i++;
					if (i == expr.length()) break;
				} while ((expr.at(i) - '0') >= 0 && (expr.at(i) - '0') <= 9 || expr.at(i) == '.');
				i--;

				if (!isNumber(final_num)) error("Syntax error!");

				out_queue.push_back(final_num);
			}
			//Case operator
			else if (token == '+' || token == '-' || token == '*' || token == '/' || token == '^')
			{
				if (op_stack.size() > 0)
					while (higherOrEqPrecOp(op_stack.top(), token))
					{
						out_queue.push_back(string(1, op_stack.top()));
						op_stack.pop();

						if (op_stack.size() == 0) break; //Exit if there are no elements in stack
					}

				op_stack.push(token);
			}
			//Case left bracket
			else if (token == '(')
			{
				//GET THE CONTENT
				string content = searchClosedPar(expr, i); //Parenthesis content

				vector<string> postfix_content = parseInfix(content);
				out_queue.insert(out_queue.end(), postfix_content.begin(), postfix_content.end());
			}
			//Unknow
			else error("Unknown token!");
		}
	}

	//Out all ops
	while (op_stack.size() > 0)
	{
		out_queue.push_back(string(1, op_stack.top()));
		op_stack.pop();
	}

	return out_queue;
}
double parsePostfix(vector<string> postfix_expr, double x)
{
	/* Compute postfix */
	//vars
	stack<double> nums;

	//code
	for (unsigned int i = 0; i < postfix_expr.size(); i++) {
		char token = postfix_expr[i].at(0);

		//function case
 		if (isalpha(token) && postfix_expr[i].length() > 1)
		{
			string func = postfix_expr[i];
			i++;

			unsigned int stop_pos = stoi(postfix_expr[i].c_str()) + i;
			i++;

			vector<string> content;
			for (i; i <= stop_pos; i++)
				content.push_back(postfix_expr[i]);
			i--;

			double argument = parsePostfix(content, x);

			if (func == "abs") nums.push(abs(argument));
			else if (func == "cos") nums.push(cos(argument));
			else if (func == "sin") nums.push(sin(argument));
			else if (func == "tan") nums.push(tan(argument));
			else if (func == "acos") nums.push(acos(argument));
			else if (func == "asin") nums.push(asin(argument));
			else if (func == "atan") nums.push(atan(argument));
			else if (func == "cosh") nums.push(cosh(argument));
			else if (func == "sinh") nums.push(sinh(argument));
			else if (func == "tanh") nums.push(tanh(argument));
			else if (func == "acosh") nums.push(acosh(argument));
			else if (func == "asinh") nums.push(asinh(argument));
			else if (func == "atanh") nums.push(atanh(argument));
			else if (func == "sqrt") nums.push(sqrt(argument));
			else if (func == "cbrt") nums.push(cbrt(argument));
			else if (func == "exp") nums.push(exp(argument));
			else if (func == "ln") nums.push(log(argument));
			else error("Unknokn function!");
		}
		else
		{
			//case variable
			if (token == 'x')
				nums.push(x);
			else if (token == 'e')
				nums.push(E);
			else if (token == 'p')
				nums.push(PI);

			//Case number
			else if ((token - '0') >= 0 && (token - '0') <= 9)
				nums.push(atof(postfix_expr[i].c_str()));

			//Case operator
			else
			{
				double a, b, result;

				if (nums.size() < 2)
					error("Syntax error!");

				b = nums.top();
				nums.pop();
				a = nums.top();
				nums.pop();

				switch (token)
				{
				case '+':
					result = a + b;
					break;
				case '-':
					result = a - b;
					break;
				case '*':
					result = a * b;
					break;
				case '/':
					if (b == 0) error("Zero division!");
					result = a / b;
					break;
				case '^':
					result = pow(a, b);
					break;
				default:
					error("Unknown token!");
				}

				nums.push(result);
			}
		}
	}

	return nums.top();
}