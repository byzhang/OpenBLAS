/*********************************************************************/
/* Copyright 2009, 2010 The University of Texas at Austin.           */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of The University of Texas at Austin.                 */
/*********************************************************************/

#include <stdio.h>
#include <math.h>
#include "common.h"

static FLOAT dm1 = -1.;

#ifndef SQRT
#define SQRT(x)	sqrt(x)
#endif

blasint CNAME(blas_arg_t *args, BLASLONG *range_m, BLASLONG *range_n, FLOAT *sa, FLOAT *sb, BLASLONG myid) {

  BLASLONG n, lda;
  FLOAT *a;

  FLOAT ajj[2];
  BLASLONG i, j;

  n      = args -> n;
  a      = (FLOAT *)args -> a;
  lda    = args -> lda;

  if (range_n) {
    n      = range_n[1] - range_n[0];
    a     += range_n[0] * (lda + 1) * COMPSIZE;
  }

  for (j = 0; j < n; j++) {

    ajj[0] = DOTC_K(j, a, 1, a, 1);
    GET_IMAGE(ajj[1]);

    ajj[0] = *(a + j * 2) - ajj[0];

    if (ajj[0] <= 0){
      *(a + j * 2 + 0) = ajj[0];
      *(a + j * 2 + 1) = ZERO;
      return j + 1;
    }

    ajj[0] = SQRT(ajj[0]);
    *(a + j * 2 + 0)  = ajj[0];
    *(a + j * 2 + 1)  = ZERO;

    i = n - j - 1;

    if (i > 0){
      GEMV_U(j, i, 0, dm1, ZERO,
	      a + lda * 2, lda,
	      a, 1,
	      a + (j + lda) * 2, lda, sb);

      SCAL_K(i, 0, 0, ONE / ajj[0], ZERO,
	      a + (j + lda) * 2, lda, NULL, 0, NULL, 0);
    }

    a += 2 * lda;
  }

  return 0;
}
