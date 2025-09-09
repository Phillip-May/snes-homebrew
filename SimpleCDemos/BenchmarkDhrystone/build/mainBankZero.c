#include <stdint.h>

typedef enum	{Ident1, Ident2 = 10000, Ident3, Ident4, Ident5} Enumeration;

typedef char	String30[31];

struct	Record
{
	String30		StringComp;
};

void main(void) {

    while(1) {
    }
    return 0;
}

/* Cross-compiler interrupt handlers, must be present */
/* Note: These are called from assembly interrupt handlers, so they should NOT have */
/* the interrupt attribute as register saving/restoring is handled in assembly */
void snesXC_cop(void) {
}

void snesXC_brk(void) {
}

void snesXC_abort(void) {
}

void snesXC_nmi(void) {
}

void snesXC_irq(void) {
}



