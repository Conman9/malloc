#include <stdio.h>
#include "sfmm.h"

/*
 * Define WEAK_MAGIC during compilation to use MAGIC of 0x0 for debugging purposes.
 * Note that this feature will be disabled during grading.
 */
#ifdef WEAK_MAGIC
int sf_weak_magic = 1;
#endif

int main(int argc, char const *argv[]) {
/*	double *x = sf_malloc(4000);
	*x = 234;
	double *y = sf_malloc(30);
	*y = 1;
	*/
		double *w = sf_malloc(5000);
	double *x = sf_malloc(5000);
	double *y = sf_malloc(5000);
	double *z = sf_malloc(5000);

	*w = 123;
	*x = 234;
	*y = 456;
	*z = 789;
	sf_show_heap();
/*
	void *x = sf_malloc(sizeof(double) * 8);
	sf_show_heap();
	void *y = sf_realloc(x, sizeof(int));
	sf_block *bp = (sf_block *)((char*)y - 2*sizeof(sf_header));
	sf_show_heap();
	sf_show_block(bp);
*/
/*
    double* ptr1 = sf_malloc(200);
	*ptr1 = 123456789;
   //	sf_show_heap();

	double* ptr2 = sf_malloc(300);
	*ptr2 = 987654321;

	double* ptr3 = sf_malloc(200);
	*ptr3 = 987654321;

	double* ptr4 = sf_malloc(500);
	*ptr4 = 987654321;

	double* ptr5 = sf_malloc(200);
	*ptr5 = 987654321;

	double* ptr6 = sf_malloc(700);
	*ptr6 = 987654321;

    sf_free(ptr1);
    sf_free(ptr3);
   sf_free(ptr5);
   */
 //       sf_free(ptr4);

         //   sf_show_heap();

   //	double* ptr3 = sf_malloc(200);
// *ptr3 = 987654321;

	//free(ptr);
	//free(ptr2);
	//sf_show_heap();
    return EXIT_SUCCESS;
}
