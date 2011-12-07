/* sntable.c - sntable */

#include <conf.h>
#include <kernel.h>
#include <network.h>
#include <snmp.h>
#include <mib.h>

/*------------------------------------------------------------------------
 * sntable - call function to operate on a table-embedded variable
 *------------------------------------------------------------------------
 */
int sntable(bindl, mip, op)
struct snbentry	*bindl;
struct mib_info	*mip;
int		op;
{
	int 	numifaces = Net.nif - 1;
	struct tab_info		*tptr;

	/*
	 * mip->mi_param holds a pointer to an entry in tabtab that
	 * contains the pointers to functions for each table
	 */
	switch (op) {
	case SOP_GET:
		tptr = (struct tab_info *)mip->mi_param;
		return (tptr->ti_get)(bindl, numifaces);
	case SOP_GETF:
		tptr = (struct tab_info *)mip->mi_param;
		return (tptr->ti_getf)(bindl, mip, numifaces);
	case SOP_SET:
		tptr = (struct tab_info *)mip->mi_param;
		return (tptr->ti_set)(bindl, mip, numifaces);
	case SOP_GETN:
		tptr = (struct tab_info *)mip->mi_param;
		return (tptr->ti_getn)(bindl, mip, numifaces);
	}
	return SYSERR;
}
