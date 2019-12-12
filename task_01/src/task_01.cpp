//
// Created by vldmr on 13.09.19.
//
#include "task_01.h"


* Given grammar:
 * <Soft>        ::= program <id> ; <block> .
 * <block>       ::= <var part> <state part>
 * <var part>    ::= var <var dec> : <type> ;
 * <var dec>     ::= <id> { , <var dec> }
 * <state part>  ::= <compound>
 * <compound>    ::= begin <state> { ; <state> } end
 * <state>       ::= <assign> | <compound> | <your_other_operations>
 * <assign>      ::= <id> := <exp> ;
 * <exp>         ::= <id> | <constant>
 * <type>        ::= integer
 * <id>          ::= a-z
 * <constant>    ::= 0-9
 */

enum tokens {
	unknown_tk = -1,    // неизвестный токен
	program_tk = 0,     // 'program'
	var_tk,             // 'var'
	begin_tk,           // 'begin'
	end_tk,             // 'end'
	type_tk,            // 'type'
	id_tk = 8,    // any [aA-zZ][0-9]
	constant_tk = 9,    // 0-9
	dot_tk,             // '.'
	comma_tk,           // ','
	ddt_tk,             // ':'
	semi_tk,            // ';'
	eqv_tk,             // '='
	ass_tk,             // ':='
	add_tk,				// '+'
	sub_tk,				// '-'
	mul_tk,				// '*'
	div_tk				// '/'
};

enum errors {
	ERR_R_FILE,      //Can't read from the file
	EOF_ERR,         // End of file error
	EXC_ERR,         //Catch exception in
	UNKNOWN_TOKEN,   //Unknown token
	BACK_ERR,        //You try to peek too much forward, get back
	ERR_1,           //Must be identificator or constant or '-' or '('
	MUST_BE_ASS,     // Error: Must be ':='
	MUST_BE_SEMI,    // Error: Must be ';'
	END_ERR,         //Each begin must be closed by 'end'
	MUST_BE_DOT,     // Error: Must be '.'
	MUST_BE_ID,      // Error: Must be identifier
	MUST_BE_DDT,     //Error: Must be ':'
	MUST_BE_TYPE,    // Error: Must be type of identifier
	UNKNOWN_LEXEM,   // Unknown lexeme
	NAME_PR_ERR,     //Name of program is absent
	UTOP_ERR,        //Undefined type of program
	OPEN_ERR,        //Can't open file
	DUPL_ID_ERR,     // Error: Duplicate declaration on identifier
	UNKNOWN_ID,      // Error: Undefined identifier
	MUST_BE_COMMA,   // Error: Must be ','
	MUST_BE_PROG,    // Error: Must be 'program'
};

typedef std::pair<std::string, tokens> lexem;

typedef struct synt {
	int str = 1;
	std::ifstream code;         // input stream of Pascal code
	lexem GetLex();             // get next lexem
	lexem PeekLex(int n);       // peek N lexem;
	bool codefail();
	bool codeeof();
	void codeopen(std::string);
	bool codeisopen();
	void codeclose();
private:

	char  GetCurrentCurs();     // get current character in stream
	char  PeekChar(int n);      // peek on N symbols in stream
	char cursor{ -1 };         // cursor of stream
	char getChar();            // get next character in stream
} synt_t;

struct TListElem    ///связный список
{
	std::string Type;
	std::string Info;
	TListElem* Link;

};

struct varstr // структура для работы с переменными
{
	TListElem* variable = NULL;
	int Search_var(std::string id); // проверка наличия переменной
	void Add_var(std::string id, std::string Type); // вставка переменной
	void Delete_var();  // удаление
	
}Table_variable;

TListElem* AddInfoToList(std::string Type, std::string info, TListElem* pList) // добавление элемента в список
{
	TListElem* pElem = new TListElem;
	pElem->Type = Type;
	pElem->Info = info;
	pElem->Link = pList;

	return pElem;
}

bool SearchList(std::string id, TListElem* pList) // поиск элемента в списке 
{
	TListElem* pElem = pList;
	while (pElem != NULL)
	{
		if (pElem->Info == id)
			return 1;
		pElem = pElem->Link;
	}
	return 0;
}

void DeleteList(TListElem* &pList) // удаление списка
{
	TListElem* pNext;
	TListElem* pElem = pList;

	while (pElem != nullptr)
	{
		pNext = pElem->Link;
		delete pElem;
		pElem = nullptr;
		pElem = pNext;
	}

}

int varstr::Search_var(std::string id)
{
	return SearchList(id, variable);
}

void varstr::Add_var(std::string Type, std::string id)
{
	variable = AddInfoToList(Type, id, variable);
}

void varstr::Delete_var()
{
	DeleteList(variable);
}

int Error(errors err, int kstr)
{
	switch (err) { 
	case ERR_R_FILE:
	{
		std::cerr << "<E> Can't read from the file" << std::endl;
		break;
	}
	case EOF_ERR:
	{
		std::cerr << "<E> File is EOF early" << std::endl;
		break;
	}
	case EXC_ERR:
	{
		std::cerr << "<E> Catch exception in " << std::endl;
		break;
	}
	case UNKNOWN_TOKEN:
	{
		std::cerr << "<E> Unknown token " << std::endl;
		break;
	}
	case BACK_ERR:
	{
		std::cerr << "<E> You try to peek too much forward, get back" << std::endl;
		break;
	}
	case ERR_1:
	{
		std::cerr << "<E> Must be identificator or constant or '-' or '('" << std::endl;
		break;
	}
	case MUST_BE_ASS:
	{
		std::cerr << "<E> := is absent" << std::endl;
		break;
	}
	case MUST_BE_SEMI:
	{
		std::cerr << "<E> ; is absent" << std::endl;
		break;
	}
	case END_ERR:
	{
		std::cerr << "<E> Each begin must be closed by 'end'" << std::endl;
		break;
	}
	case MUST_BE_DOT:
	{
		std::cerr << "<E> '.' is absent" << std::endl;
		break;
	}
	case MUST_BE_ID:
	{
		std::cerr << "<E> Here must be identificator" << std::endl;
		break;
	}
	case MUST_BE_DDT:
	{
		std::cerr << "<E> : is absent" << std::endl;
		break;
	}
	case MUST_BE_TYPE:
	{
		std::cerr << "<E> Identificator must have type" << std::endl;
		break;
	}
	case UNKNOWN_LEXEM:
	{
		std::cerr << "<E> Unknown lexem" << std::endl;
		break;
	}
	case NAME_PR_ERR:
	{
		std::cerr << "<E> Name of program is absent" << std::endl;
		break;
	}
	case UTOP_ERR:
	{
		std::cerr << "<E> Undefined type of program" << std::endl;
		break;
	}
	case OPEN_ERR:
	{
		std::cerr << "<E> Can't open file" << std::endl;
		break;
	}
	case DUPL_ID_ERR:
	{
		std::cerr << "<E> This indicator is already there" << std::endl;
		break;
	}
	case UNKNOWN_ID:
	{
		std::cerr << "<E> Unknown ID" << std::endl;
		break;
	}
	}
	std::cerr << "\n" << kstr << std::endl;
	return 0;
}

/**
 * @brief Get next character (symbol)
 * @param none
 *
 * @return current character
 * @note If stream is broken (eof or something), throw exception
 */

char synt_t::getChar() {
	if (code.fail()) {
		Error(EOF_ERR, str);
		throw std::runtime_error("File doesn't available");
	}
	if (!code.eof()) {
		code >> std::noskipws >> cursor;
	}
	else {
		Error(EOF_ERR, str);
		throw std::runtime_error("File is EOF early");
	}
	return cursor;
}

/**
 * @brief Peek to forward in stream on @n symbols
 * @param[in] n - the number of symbols for peek
 *
 * @return symbol from stream
 * @return -1 - can't peek forward
 * @note Peek only forward
 */
char synt_t::PeekChar(int n) {
	try {
		char ch = -1;
		int curr_pos = code.tellg(); // get current position in stream

		code.seekg(curr_pos + n, code.beg); // set needed position in stream
		code >> std::noskipws >> ch;    // get character from stream with ' '
		code.seekg(curr_pos, code.beg); // return previous position in stream

		return ch;
	}
	catch (std::exception & exp) {
		Error(EXC_ERR, str);
		std::cerr << __func__ << ": " << exp.what()
			<< std::endl;
		return -1;
	}
}

/**
 * @brief Get current value of cursor
 * @param none
 *
 * @return value of cursor
 */
char synt_t::GetCurrentCurs() {
	return cursor;
}

/**
 * @brief Get next lexem
 * @param none
 *
 * @return lexem value (token)
 */
lexem synt_t::GetLex() {
	try {
		auto ch = GetCurrentCurs();
		while (ch == -1 || ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
			if (ch == '\n')
			{
				str = str + 1;  // str++
			}

			ch = getChar();
		}

		auto isId = [](char ch) {
			return std::isalpha(static_cast<unsigned char>(ch)) ||
				std::isdigit(static_cast<unsigned char>(ch));
		};

		std::string lex;
		if (std::isdigit(static_cast<unsigned char>(ch))) { // Constants (Numbers)
			while (std::isdigit(static_cast<unsigned char>(ch))) {
				lex += ch;
				ch = getChar();
			}
			return std::make_pair(lex, constant_tk);
		}
		else if (std::isalpha(static_cast<unsigned char>(ch))) { // Identificators
			while (isId(ch)) {
				lex += ch;
				ch = getChar();
			}
			if (lex == "program") { return std::make_pair(lex, program_tk); }
			else if (lex == "var") { return std::make_pair(lex, var_tk); }
			else if (lex == "begin") { return std::make_pair(lex, begin_tk); }
			else if (lex == "integer") { return std::make_pair(lex, type_tk); }
			else if (lex == "end") { return std::make_pair(lex, end_tk); }
			else {
				return std::make_pair(lex, id_tk); // it is ID
			}
		}
		else if (std::ispunct(static_cast<unsigned char>(ch))) { // Other symbols
			tokens tok;
			switch (ch) {
			case ',': tok = comma_tk; break;
			case '.': tok = dot_tk;   break;
			case ':': tok = ddt_tk;   break;
			case ';': tok = semi_tk;  break;
			case '=': tok = eqv_tk;   break;
			case '+': tok = add_tk;   break;
			case '-': tok = sub_tk;   break;
			case '*': tok = mul_tk;   break;
			case '/': tok = div_tk;   break;
			default:
				Error(UNKNOWN_TOKEN, str);
				std::cerr << ch << std::endl; break;
			}
			lex += ch;

			if (tok == ddt_tk) {
				ch = getChar();
				if (ch == '=') {
					lex += ch;
					tok = ass_tk;
				}
			}
			getChar(); // some kind of k o s t y l; here we look on \n
			return std::make_pair(lex, tok);
		}
		else {
			Error(UNKNOWN_TOKEN, str);
			std::cerr  << ch << std::endl;  
		}

		return std::make_pair("", unknown_tk);
	}
	catch (const std::exception & exp) {
		return std::make_pair("", unknown_tk);
	}
}


/**
 * @brief Peek to forward on the N lexem
 * @param[in] n - the number of skipped tokens
 *
 * @return N-th lexem (token)
 * @note Return cursor of stream to previous position
 */
lexem synt_t::PeekLex(int n) {
	int curr_pos = code.tellg(); // get current position in stream
	auto curr_curs = GetCurrentCurs();
	try {
		lexem res;
		for (int i = 0; i < n; i++) {
			res = GetLex();
		}
		code.seekg(curr_pos, code.beg);
		cursor = curr_curs;

		return res;
	}
	catch (const std::exception & exp) {
		Error(BACK_ERR, str);
		code.seekg(curr_pos, code.beg);
		cursor = curr_curs;

		return std::make_pair("", unknown_tk);
	}
}

/********** prototypes of functions ***************/

void  buildTreeStub(lexem lex);
int   expressionParse(lexem lex, synt_t& parser);
int   stateParse(lexem& lex, synt_t& parser);
int   compoundParse(lexem lex, synt_t& parser);
lexem vardParse(lexem lex, synt_t& parser);
int   blockParse(lexem lex, synt_t& parser);
int   programParse(synt_t& parser);
/*************************************************/


/**
 * @brief Stub for building tree
 * @param[in] lex - current lexem
 *
 * @return none
 * @note: in future here you will be building tree node
 */
void buildTreeStub(lexem lex) {
	std::cout << "<D> stub, get lexem " << lex.first << " (" << lex.second << ")"
		<< std::endl;
}


/**
 * @brief Parse expression part
 * @param[in]    lex    - current lexem
 * @param[inout] parser - our lexical and syntax analyzer
 *
 * @return  EXIT_SUCCESS - if expression part is matched to grammar
 * @return -EXIT_FAILURE - if expression part doesn't matched to grammar
 */

int expressionParse(lexem lex, synt_t& parser) {
	bool sr;
	lex = parser.GetLex();

	switch (lex.second) {
	case id_tk:
	case constant_tk: {
		sr = Table_variable.Search_var(lex.first);
		if (sr == 1)
		{
			lex = parser.PeekLex(1);
			if (lex.second == add_tk || lex.second == sub_tk || lex.second == mul_tk || lex.second == div_tk)
			{
				lex = parser.GetLex();
				buildTreeStub(lex);
				expressionParse(lex, parser);
			}
			
		}
		else Error(UNKNOWN_ID, parser.str);
	}break;
	default: {
		Error(ERR_1, parser.str);
		return -EXIT_FAILURE;
	}
	}

	return EXIT_SUCCESS;
}


/**
 * @brief Parse state part
 * @param[in]    lex    - current lexem
 * @param[inout] parser - our lexical and syntax analyzer
 *
 * @return  EXIT_SUCCESS - if state part is matched to grammar
 * @return -EXIT_FAILURE - if state part doesn't matched to grammar
 */
int stateParse(lexem& lex, synt_t& parser) {
	lex = parser.GetLex();
	bool sr;
	switch (lex.second) {
	case id_tk: { 	 
		sr = Table_variable.Search_var(lex.first);
		if ( sr == 0)
		{
			return -EXIT_FAILURE;
		}

		lex = parser.GetLex();
		if (lex.second != ass_tk) {
			Error(MUST_BE_ASS, parser.str);
			return -EXIT_FAILURE;
		}

		expressionParse(lex, parser);

		lex = parser.GetLex();
		if (lex.second != semi_tk) {
			Error(MUST_BE_SEMI, parser.str);
			return -EXIT_FAILURE;
		}
		break;
	}
	case begin_tk: { // compound statements
		compoundParse(lex, parser);
		lex = parser.GetLex();
		if (lex.second != semi_tk) {
			Error(MUST_BE_SEMI, parser.str);
			return -EXIT_FAILURE;
		}
		break;
	}
	default: {

		break;
	}
	}

	return EXIT_SUCCESS;
}


/**
 * @brief Parse compound part
 * @param[in]    lex    - current lexem
 * @param[inout] parser - our lexical and syntax analyzer
 *
 * @return  EXIT_SUCCESS - if compound part is matched to grammar
 * @return -EXIT_FAILURE - if compound part doesn't matched to grammar
 */
int compoundParse(lexem lex, synt_t& parser) {
	static int compound_count = 0;
	compound_count++;
	while (lex.second != end_tk) {
		buildTreeStub(lex); // Here is your Tree realization
		if (parser.code.eof()) {
			Error(END_ERR, parser.str);
			return -EXIT_FAILURE;
		}
		stateParse(lex, parser);
	}

	if (compound_count == 1) {
		if (parser.PeekLex(1).second == unknown_tk) {
			Error(MUST_BE_DOT, parser.str);
			return -EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}


/**
 * @brief Parse variable declaration part
 * @param[in]    lex    - current lexem
 * @param[inout] parser - our lexical and syntax analyzer
 *
 * @return lexem - last parsed lexem (will actually return ':')
 */
lexem vardParse(lexem lex, synt_t& parser) {
	lex = parser.GetLex();
	if (lex.second != id_tk) {
		Error(MUST_BE_ID, parser.str);
		return lex;
	}

	int lxm = 0;
	lexem type;
	bool sr;
	sr = Table_variable.Search_var(lex.first);
	if (sr == 0)
	{
		do
		{
		lxm = lxm ++;
			type = parser.PeekLex(lxm + 1);
			if (type.second == type_tk)
				Table_variable.Add_var(type.first, lex.first);
		
		} while (type.second != semi_tk && type.second != type_tk);
		std::cerr << "Table of lexem\n" << Table_variable.variable->Info << std::endl;
	}
	else Error(DUPL_ID_ERR, parser.str);

	lex = parser.GetLex();
	if (lex.second == comma_tk)
		return vardParse(lex, parser); // Раскручивая стек обратно,
									   // будет возвращено последнее значение

	return lex;
}


/**
 * @brief Parse block part
 * @param[in]    lex    - current lexem
 * @param[inout] parser - our lexical and syntax analyzer
 *
 * @return  EXIT_SUCCESS - if block part is matched to grammar
 * @return -EXIT_FAILURE - if block part doesn't matched to grammar
 */
int blockParse(lexem lex, synt_t& parser) {
	lex = parser.GetLex();
	switch (lex.second) { // var / begin
	case var_tk: {   // var a, b: integer;
		lex = vardParse(lex, parser);
		if (lex.second != ddt_tk)
			Error(MUST_BE_DDT, parser.str);

		lex = parser.GetLex();
		if (lex.second != type_tk)
			Error(MUST_BE_TYPE, parser.str);

		lex = parser.GetLex();
		if (lex.second != semi_tk) {
			Error(MUST_BE_SEMI, parser.str);
			return -EXIT_FAILURE;
		}

		buildTreeStub(lex); // Here is your Tree realization

		break;
	}
	case begin_tk: {
		compoundParse(lex, parser);
		break;
	}
	case dot_tk: {
		std::cout << "Program was parse successfully" << std::endl;
		return 1;
	}
	default: {
		Error(UNKNOWN_LEXEM, parser.str);
		return -EXIT_FAILURE;
	}
	}

	return EXIT_SUCCESS;
}




/**
 * @brief Entry point in grammar
 * @param[inout] parser - our lexical and syntax analyzer
 *
 * @return  EXIT_SUCCESS - if input program part is correct
 * @return -EXIT_FAILURE - if input program part is incorrect
 * @note Wait input like: program <id_tk> ;
 */
int programParse(synt_t& parser) {
	auto lex = parser.GetLex();
	if (lex.second == program_tk) {
		lex = parser.GetLex();
		if (lex.second != id_tk) {
			Error(NAME_PR_ERR, parser.str);
			if (lex.second != semi_tk) {
				Error(MUST_BE_SEMI, parser.str);
				return -EXIT_FAILURE;
			}
		}

		lex = parser.GetLex();
		if (lex.second != semi_tk) {
			Error(MUST_BE_SEMI, parser.str);
			return -EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}
	else {
		Error(UTOP_ERR, parser.str);
		return -EXIT_FAILURE;
	}
}


/**
 * @brief Start parse incoming pascal file
 * @param[in] file_path - path to a pascal file (code.p)
 *
 * @return  EXIT_SUCCESS - if file was successful parsed
 * @return -EXIT_FAILURE - if can't parse incoming file
 */
int Parse2(const std::string& file_path) {
	try {
		synt_t example_synt;

		example_synt.code.open(file_path);
		if (!example_synt.code.is_open()) {
			Error(OPEN_ERR, 0);
			return -EXIT_FAILURE;
		}

		if (programParse(example_synt) != 0) {
			example_synt.code.close();
			return -EXIT_FAILURE;
		}

		lexem lex;
		while (!example_synt.code.eof() && !example_synt.code.fail()) {
			if (blockParse(lex, example_synt) == 1)
				break;
		}

		example_synt.code.close();
		return EXIT_SUCCESS;
	}
	catch (const std::exception & exp) {
		Error(EXC_ERR, 0);
		std::cerr << __func__ << ": " << exp.what()
			<< std::endl;
		return -EXIT_FAILURE;
	}
}



// работа с файлами
bool synt_t::codefail()
{
	return code.fail();
}

bool synt_t::codeeof()
{
	return code.eof();
}

void synt_t::codeopen(std::string doc)
{
	code.open(doc);
}

bool synt_t::codeisopen()
{
	return code.is_open();
}

void synt_t::codeclose()
{
	code.close();
}

/**
 * What you have to do:
 *  - rewrite synt structure (set more encapsulated);
 *  - add struct for work with variables;
 *  - add variable check (check of existen);
 *  - add + and - operations (with variadic number of values);
 *  - add function of error information (must pass the number line of code
 *      with error, type of error);
 *  - rewrite work with table of lexemes (get lexemes directly from table);
 *  - implement all TODO tips (start with 'TODO: Here you have to ...')
 */
