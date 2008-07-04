/****************************************************************************
 *                           The Twobit algorithm                           *
 *                   for constant width DNA dictionaries                    *
 *                                                                          *
 *                           Author: Herve Pages                            *
 *                                                                          *
 * Note: a constant width dictionary is a non-empty set of non-empty        *
 * words of the same length.                                                *
 ****************************************************************************/
#include "Biostrings.h"

static int debug = 0;


SEXP debug_match_pdict_Twobit()
{
#ifdef DEBUG_BIOSTRINGS
	debug = !debug;
	Rprintf("Debug mode turned %s in 'match_pdict_Twobit.c'\n",
		debug ? "on" : "off");
#else
	Rprintf("Debug mode not available in 'match_pdict_Twobit.c'\n");
#endif
	return R_NilValue;
}



/****************************************************************************
 *                                                                          *
 *                             A. PREPROCESSING                             *
 *                                                                          *
 ****************************************************************************/

static int eightbit2twobit_lkup[256];

static void init_twobit_sign2pos(SEXP twobit_sign2pos, int val0)
{
	int i;

	for (i = 0; i < LENGTH(twobit_sign2pos); i++)
		INTEGER(twobit_sign2pos)[i] = val0;
	return;
}

static int pp_pattern(SEXP twobit_sign2pos, const RoSeq *pattern, int poffset)
{
	int twobit_sign, i, twobit, *pos0;
	const char *c;

	twobit_sign = 0;
	//printf("poffset=%d: ", poffset);
	for (i = 0, c = pattern->elts; i < pattern->nelt; i++, c++) {
		twobit = eightbit2twobit_lkup[(unsigned char) *c];
		//printf("(%d %d) ", *c, twobit);
		if (twobit == -1)
			return -1;
		twobit_sign <<= 2;
		twobit_sign += twobit;
	}
	//printf("twobit_sign=%d\n", twobit_sign);
	pos0 = INTEGER(twobit_sign2pos) + twobit_sign;
	if (*pos0 == NA_INTEGER)
		*pos0 = poffset + 1;
	else
		_report_dup(poffset, *pos0);
	return 0;
}


/****************************************************************************
 * Turning our local data structures into an R list (SEXP)
 * -------------------------------------------------------
 */

static SEXP new_ExternalPtr_from_twobit_sign2pos(SEXP twobit_sign2pos)
{
	SEXP ans;

	PROTECT(ans = R_MakeExternalPtr(NULL, R_NilValue, R_NilValue));
	R_SetExternalPtrTag(ans, twobit_sign2pos);
	UNPROTECT(1);
	return ans;
}

/*
 * Twobit_asLIST() returns an R list with the following elements:
 *   - twobit_sign2pos_xp: 
 *   - dup2unq: an integer vector containing the mapping between duplicated and
 *         primary reads.
 */

static SEXP Twobit_asLIST(SEXP twobit_sign2pos)
{
	SEXP ans, ans_names, ans_elt;

	PROTECT(ans = NEW_LIST(2));

	/* set the names */
	PROTECT(ans_names = NEW_CHARACTER(2));
	SET_STRING_ELT(ans_names, 0, mkChar("twobit_sign2pos_xp"));
	SET_STRING_ELT(ans_names, 1, mkChar("dup2unq"));
	SET_NAMES(ans, ans_names);
	UNPROTECT(1);

	/* set the "twobit_sign2pos_xp" element */
	PROTECT(ans_elt = new_ExternalPtr_from_twobit_sign2pos(twobit_sign2pos));
	SET_ELEMENT(ans, 0, ans_elt);
	UNPROTECT(1);

	/* set the "dup2unq" element */
	PROTECT(ans_elt = _dup2unq_asINTEGER());
	SET_ELEMENT(ans, 1, ans_elt);
	UNPROTECT(1);

	UNPROTECT(1);
	return ans;
}


/****************************************************************************
 * .Call entry point for preprocessing
 * -----------------------------------
 *
 * Arguments:
 *   tb: the Trusted Band extracted from the original dictionary as a
 *       DNAStringSet object;
 *   base_codes: the internal codes for A, C, G and T.
 *
 * See Twobit_asLIST() for a description of the returned SEXP.
 */

SEXP build_Twobit(SEXP tb, SEXP base_codes)
{
	int tb_length, tb_width, poffset, twobit_len;
	CachedXStringSet cached_tb;
	RoSeq pattern;
	SEXP ans, twobit_sign2pos;

	if (LENGTH(base_codes) != 4)
		error("Biostrings internal error in build_Twobit(): "
		      "'base_codes' must be of length 4");
	_init_chrtrtable(INTEGER(base_codes), LENGTH(base_codes),
			 eightbit2twobit_lkup);
	tb_length = _get_XStringSet_length(tb);
	_init_dup2unq_buf(tb_length);
	tb_width = -1;
	cached_tb = _new_CachedXStringSet(tb);
	for (poffset = 0; poffset < tb_length; poffset++) {
		pattern = _get_CachedXStringSet_elt_asRoSeq(&cached_tb,
				poffset);
		if (pattern.nelt == 0)
			error("empty trusted region for pattern %d",
			      poffset + 1);
		if (tb_width == -1) {
			tb_width = pattern.nelt;
			if (tb_width > 14)
				error("the width of the Trusted Band must "
				      "be <= 14 when 'type=\"Twobit\"'");
			twobit_len = 1 << (tb_width * 2); // 4^tb_width
			PROTECT(twobit_sign2pos = NEW_INTEGER(twobit_len));
			init_twobit_sign2pos(twobit_sign2pos, NA_INTEGER);
		} else if (pattern.nelt != tb_width) {
			error("all the trusted regions must have "
			      "the same length");
		}
		if (pp_pattern(twobit_sign2pos, &pattern, poffset) != 0) {
			UNPROTECT(1);
			error("non-base DNA letter found in Trusted Band "
			      "for pattern %d", poffset + 1);
		}
			
	}
	PROTECT(ans = Twobit_asLIST(twobit_sign2pos));
	UNPROTECT(2);
	return ans;
}



/****************************************************************************
 *                                                                          *
 *                             B. MATCH FINDING                             *
 *                                                                          *
 ****************************************************************************/

void walk_subject(int tb_width, const int *twobit_sign2pos, const RoSeq *S)
{
	int bitwindow_width, bitmask, nb_valid_left_char,
	    n, twobit, twobit_sign, P_id;
	const char *s;

	bitwindow_width = (tb_width - 1) * 2;
	bitmask = (1 << bitwindow_width) - 1;
	nb_valid_left_char = 0;
	for (n = 1, s = S->elts; n <= S->nelt; n++, s++) {
		twobit = eightbit2twobit_lkup[(unsigned char) *s];
		if (twobit == -1) {
			nb_valid_left_char = 0;
			continue;
		}
		nb_valid_left_char++;
		twobit_sign &= bitmask;
		twobit_sign <<= 2;
		twobit_sign += twobit;
		if (nb_valid_left_char < tb_width)
			continue;
		P_id = twobit_sign2pos[twobit_sign];
		if (P_id == NA_INTEGER)
			continue;
		_MIndex_report_match(P_id - 1, n);
	}
	return;
}

void _match_Twobit(SEXP pdict_pptb, const RoSeq *S, int fixedS)
{
	int tb_width;
	const int *twobit_sign2pos;
	SEXP base_codes;

#ifdef DEBUG_BIOSTRINGS
	if (debug)
		Rprintf("[DEBUG] ENTERING _match_Twobit()\n");
#endif
	tb_width = INTEGER(VECTOR_ELT(pdict_pptb, 0))[0];
	twobit_sign2pos = INTEGER(R_ExternalPtrTag(VECTOR_ELT(pdict_pptb, 1)));
	base_codes = VECTOR_ELT(pdict_pptb, 2);
	if (LENGTH(base_codes) != 4)
		error("Biostrings internal error in _match_Twobit(): "
		      "'base_codes' must be of length 4");
	_init_chrtrtable(INTEGER(base_codes), LENGTH(base_codes),
			 eightbit2twobit_lkup);
	if (!fixedS)
		error("cannot treat IUPAC extended letters in the subject "
		      "as ambiguities when 'pdict' is a PDict object of "
		      "the \"Twobit\" type");
	walk_subject(tb_width, twobit_sign2pos, S);
#ifdef DEBUG_BIOSTRINGS
	if (debug)
		Rprintf("[DEBUG] LEAVING _match_Twobit()\n");
#endif
	return;
}
