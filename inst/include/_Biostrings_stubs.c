#include "Biostrings_interface.h"

typedef CharBuf (*new_CharBuf_from_string_FUNTYPE)(const char *);
CharBuf new_CharBuf_from_string(const char *string)
{
	static new_CharBuf_from_string_FUNTYPE fun = NULL;

	if (fun == NULL)
		fun = (new_CharBuf_from_string_FUNTYPE)
			R_GetCCallable("Biostrings", "_new_CharBuf_from_string");
	return fun(string);
}

typedef CharBBuf (*new_CharBBuf_FUNTYPE)(int, int);
CharBBuf new_CharBBuf(int buflength, int nelt)
{
	static new_CharBBuf_FUNTYPE fun = NULL;

	if (fun == NULL)
		fun = (new_CharBBuf_FUNTYPE)
			R_GetCCallable("Biostrings", "_new_CharBBuf");
	return fun(buflength, nelt);
}

typedef void (*append_string_to_CharBBuf_FUNTYPE)(CharBBuf *, const char *);
void append_string_to_CharBBuf(CharBBuf *cbbuf, const char *string)
{
	static append_string_to_CharBBuf_FUNTYPE fun = NULL;

	if (fun == NULL)
		fun = (append_string_to_CharBBuf_FUNTYPE)
			R_GetCCallable("Biostrings", "_append_string_to_CharBBuf");
	return fun(cbbuf, string);
}

char DNAencode(char c)
{
	static char (*fun)(char) = NULL;

	if (fun == NULL)
		fun = (char (*)(char)) R_GetCCallable("Biostrings", "_DNAencode");
	return fun(c);
}

char DNAdecode(char code)
{
	static char (*fun)(char) = NULL;

	if (fun == NULL)
		fun = (char (*)(char)) R_GetCCallable("Biostrings", "_DNAdecode");
	return fun(code);
}

char RNAencode(char c)
{
	static char (*fun)(char) = NULL;

	if (fun == NULL)
		fun = (char (*)(char)) R_GetCCallable("Biostrings", "_RNAencode");
	return fun(c);
}

char RNAdecode(char code)
{
	static char (*fun)(char) = NULL;

	if (fun == NULL)
		fun = (char (*)(char)) R_GetCCallable("Biostrings", "_RNAdecode");
	return fun(code);
}

const char *get_XString_charseq(SEXP x, int *length)
{
	static const char *(*fun)(SEXP, int *) = NULL;

	if (fun == NULL)
		fun = (const char *(*)(SEXP, int *)) R_GetCCallable("Biostrings", "_get_XString_charseq");
	return fun(x, length);
}

int get_XStringSet_length(SEXP x)
{
	static int (*fun)(SEXP) = NULL;

	if (fun == NULL)
		fun = (int (*)(SEXP)) R_GetCCallable("Biostrings", "_get_XStringSet_length");
	return fun(x);
}

const char *get_XStringSet_charseq(SEXP x, int i, int *nchar)
{
	static const char *(*fun)(SEXP, int, int *) = NULL;

	if (fun == NULL)
		fun = (const char *(*)(SEXP, int, int *)) R_GetCCallable("Biostrings", "_get_XStringSet_charseq");
	return fun(x, i, nchar);
}

typedef SEXP (*new_XStringSet_from_RoSeqs_FUNTYPE)(const char *, RoSeqs);
SEXP new_XStringSet_from_RoSeqs(const char *baseClass, RoSeqs seqs)
{
	static new_XStringSet_from_RoSeqs_FUNTYPE fun = NULL;

	if (fun == NULL)
		fun = (new_XStringSet_from_RoSeqs_FUNTYPE)
			R_GetCCallable("Biostrings", "_new_XStringSet_from_RoSeqs");
	return fun(baseClass, seqs);
}

typedef void (*set_XStringSet_names_FUNTYPE)(SEXP, SEXP);
void set_XStringSet_names(SEXP x, SEXP names)
{
	static set_XStringSet_names_FUNTYPE fun = NULL;

	if (fun == NULL)
		fun = (set_XStringSet_names_FUNTYPE)
			R_GetCCallable("Biostrings", "_set_XStringSet_names");
	return fun(x, names);
}

typedef SEXP (*alloc_XStringSet_FUNTYPE)(const char *, int, int);
SEXP alloc_XStringSet(const char *baseClass, int length, int super_length)
{
	static alloc_XStringSet_FUNTYPE fun = NULL;

	if (fun == NULL)
		fun = (alloc_XStringSet_FUNTYPE)
			R_GetCCallable("Biostrings", "_alloc_XStringSet");
	return fun(baseClass, length, super_length);
}

typedef void (*write_RoSeq_to_XStringSet_elt_FUNTYPE)(SEXP, int, RoSeq);
void write_RoSeq_to_XStringSet_elt(SEXP x, int i, RoSeq seq)
{
	static write_RoSeq_to_XStringSet_elt_FUNTYPE fun = NULL;

	if (fun == NULL)
		fun = (write_RoSeq_to_XStringSet_elt_FUNTYPE)
			R_GetCCallable("Biostrings", "_write_RoSeq_to_XStringSet_elt");
	return fun(x, i, seq);
}

typedef RoSeqs (*new_RoSeqs_from_BBuf_FUNTYPE)(CharBBuf);
RoSeqs new_RoSeqs_from_BBuf(CharBBuf cbbuf)
{
	static new_RoSeqs_from_BBuf_FUNTYPE fun = NULL;

	if (fun == NULL)
		fun = (new_RoSeqs_from_BBuf_FUNTYPE)
			R_GetCCallable("Biostrings", "_new_RoSeqs_from_BBuf");
	return fun(cbbuf);
}

typedef SEXP (*new_STRSXP_from_RoSeqs_FUNTYPE)(RoSeqs, SEXP);
SEXP new_STRSXP_from_RoSeqs(RoSeqs seqs, SEXP lkup)
{
	static new_STRSXP_from_RoSeqs_FUNTYPE fun = NULL;

	if (fun == NULL)
		fun = (new_STRSXP_from_RoSeqs_FUNTYPE)
			R_GetCCallable("Biostrings", "_new_STRSXP_from_RoSeqs");
	return fun(seqs, lkup);
}

void init_match_reporting(int mrmode)
{
	static void (*fun)(int) = NULL;

	if (fun == NULL)
		fun = (void (*)(int)) R_GetCCallable("Biostrings", "_init_match_reporting");
	return fun(mrmode);
}

int report_match(int start, int end)
{
	static int (*fun)(int, int) = NULL;

	if (fun == NULL)
		fun = (int (*)(int, int)) R_GetCCallable("Biostrings", "_report_match");
	return fun(start, end);
}

SEXP reported_matches_asSEXP()
{
	static SEXP (*fun)() = NULL;

	if (fun == NULL)
		fun = (SEXP (*)()) R_GetCCallable("Biostrings", "_reported_matches_asSEXP");
	return fun();
}

